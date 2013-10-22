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

	Socket(const std::string& host, const unsigned short port);
	Socket(const Socket& rhs);
	Socket& operator=(const Socket& rhs);
	~Socket();
	// close read/write/all ends of socket predestructor
	void close(SocketEnd type = All);

	// get the remote hostname
	std::string remoteHost() const;
	// read {size} bytes from socket (if size is -1 read first available amount of bytes)
	std::string read(long size = -1);
	// write buffer to socket
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
	ServerSocket(const short port = 0);
	~ServerSocket();

	// Abstracts listening socket accept functionality
	Socket accept();
private:
	int fd;
};

#endif
