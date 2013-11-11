#ifndef SOCKETWRAPPER_H
#define SOCKETWRAPPER_H
#include <string>

enum SocketEnd
{
	Read = 1,
	Write = 2,
	All = 3
};

class Socket
{
public:
	static void IgnoreSIGPIPE();

	// Throws when: unable to resolve host, cannot allocate fd (socket), connect error, connection timeout or see manpages for gethostbyname, socket, connect
	Socket(const std::string& host, const unsigned short port);
	Socket(const Socket& rhs);
	Socket& operator=(const Socket& rhs);
	~Socket();
	// close read/write/all ends of socket before the destructor
	void close(SocketEnd type = All);

	// get the remote hostname
	std::string remoteHost() const;
	// read {size} bytes from socket (if size is -1 read first available amount of bytes)
	//     	Throws when: read end is shutdown or see `man 2 read`
	std::string read(long size = -1);
	// write buffer to socket
	// 		Throws when: other side is closed, network down or see `man 2 write`
	void write(const std::string& buffer);
private:
	friend class ServerSocket;
	Socket(int fileDesc, string remote);

	int fd;
	int *ref;
	std::string inBuffer, remote;
};

class ServerSocket
{
public:
	// Throws when: cannot allocate fd (socket), cannot bind to port or see manpages for socket, bind, listen
	ServerSocket(const short port = 0);
	~ServerSocket();

	// Abstracts listening socket accept functionality
	// 		Throws when: see `man 2 accept`
	Socket accept();
private:
	int fd;
};

#endif
