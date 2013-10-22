#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace std;
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include "socket.h"

// stdint.h is missing solaris 9-10 =[
#ifdef __sun
#	include <inttypes.h>
#else
#	include <stdint.h>
#endif

void Socket::IgnoreSIGPIPE()
{
	::signal(SIGPIPE, SIG_IGN);
}

const int BUF_LENGTH = 1024;

Socket::Socket(const string& host, const uint16_t port) : fd(-1), ref(0)
{
	Socket::IgnoreSIGPIPE();
	sockaddr_in addr;
	hostent *haddr = 0;

	haddr = ::gethostbyname(host.c_str());
	if (haddr == 0)
	{
		throw runtime_error(string("Socket error (resolve): ") + strerror(errno));
	}

	fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		throw runtime_error(string("Socket error (sock): ") + strerror(errno));
	}
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);// Set the socket to async so that we can have a shorter timeout

	memset((char*)&addr, 0, sizeof(addr));
	memcpy(&addr.sin_addr.s_addr, haddr->h_addr, haddr->h_length);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (::connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1 && errno != EINPROGRESS)
	{
		::close(fd);
		throw runtime_error(string("Socket error (conn): ") + strerror(errno));
	}

	// Use a timeout of 2 seconds
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);
	timeval timeout;
	memset((char*)&timeout, 0, sizeof(timeout));
	timeout.tv_sec = 2;

	if (::select(fd + 1, 0, &fdset, 0, &timeout) == -1)
	{
		bool bad = true;
		if (errno == EINPROGRESS)
		{
			int errc;
			socklen_t len = sizeof(errc);

			getsockopt(fd, SOL_SOCKET, SO_ERROR, &errc, &len);

			if (errc == 0)
			{
				bad = false;
			}
		}
		if (bad || errno == EINPROGRESS)
		{
			::close(fd);
			throw runtime_error(string("Socket error (conn): ") + strerror(errno));
		}

	}
    fcntl(fd, F_SETFL, flags);// unset the async socket

	remote = host;
	ref = new int(1);
}

Socket::Socket(int fdes, string Remote) : fd(fdes), ref(0), remote(Remote)
{
//	cerr << "socket new : " << fd << endl;
	ref = new int(1);
}

Socket::Socket(const Socket& rhs) : fd(rhs.fd), ref(rhs.ref), remote(rhs.remote)
{
//	cerr << "socket copy: " << fd << endl;
	++(*ref);
}

Socket& Socket::operator=(const Socket& rhs)
{
	Socket::IgnoreSIGPIPE();
	if (fd != -1)
	{
		--(*ref);
		if (*ref <= 1)
		{
			delete ref;
			::close(fd);
		}
	}
//	cerr << "socket equa: " << fd << endl;
	fd = rhs.fd;
	ref = rhs.ref;
	remote = rhs.remote;
	++(*ref);
	return *this;
}

Socket::~Socket()
{
	--(*ref);
//	cerr << "socket rem : " << fd << " " << *ref << endl;
	if (*ref == 0)
	{
		delete ref;
//		cerr << "socket disp: " << fd << " " << *ref << endl;
		::close(fd);
	}
}

void Socket::close(SocketEnd type)
{
	if ((type & Read) == Read)
	{
		::shutdown(fd, SHUT_RD);
	}
	if ((type & Write) == Write)
	{
		::shutdown(fd, SHUT_WR);
	}
}

string Socket::remoteHost() const
{
	return remote;
}

string Socket::read(long size)
{
	int recvChars, read = inBuffer.length();
	char buffer[BUF_LENGTH];
	string output;

	if (size == -1 || size > (long)inBuffer.length())
	{
		do
		{
			recvChars = ::recv(fd, buffer, BUF_LENGTH, 0);
			if (recvChars > 0)
			{
				read += recvChars;
				inBuffer.append(buffer, recvChars);
			}
		}
		while (recvChars >= 0 && read < size);
	}

	if (!inBuffer.empty() && recvChars == -1)
	{
		throw runtime_error(string("Socket error (read): ") + strerror(errno));
	}


	if (size > read)
	{
		output = inBuffer.substr(0, size);
		inBuffer.erase(0, size);
	}
	else
	{
		output = inBuffer;
		inBuffer.clear();
	}

	return output;
}

void Socket::write(const string& buffer)
{
	if (::send(fd, buffer.data(), buffer.length(), 0) == -1)
	{
		throw runtime_error(string("Socket error (write): ") + strerror(errno));
	}
}

ServerSocket::ServerSocket(const short port)
{
	Socket::IgnoreSIGPIPE();
	sockaddr_in addr;

	fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		throw runtime_error(string("Socket error (sock): ") + strerror(errno));

	memset((char*)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if (::bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1)
		throw runtime_error(string("Socket error (bind): ") + strerror(errno));

	if (::listen(fd, 5) == -1)
		throw runtime_error(string("Socket error (listen): ") + strerror(errno));
}

ServerSocket::~ServerSocket()
{
	close(fd);
}

Socket ServerSocket::accept()
{
	Socket::IgnoreSIGPIPE();
	sockaddr_in in;
	socklen_t len = sizeof(in);
	int newSock = ::accept(fd, (sockaddr*)&in, &len);
	if (newSock == -1)
		throw runtime_error(string("Socket error (accept): ") + strerror(errno));

	return Socket(newSock, inet_ntoa(in.sin_addr));
}

