#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;
#include "socket.h"

Socket create_socket(string host, short port)
{
    try 
    {
        Socket sock(host, port);
        return sock;
    }
    catch (exception& e)
    {
        cerr << "Socket error: " << e.what() << endl;
        exit(1);
    }
}

int main (int argc, char** argv)
{
    string host = "www.google.com",
           path = "/";
    if (argc > 1)
        host = argv[1];
    if (argc > 2)
        path = argv[2];

    Socket sock = create_socket(host, 80);
    sock.write("GET " + path + " HTTP/1.1\r\n\
Host: " + host + "\r\n\
User-Agent: venuatu httpclient\r\n\
Connection: close\r\n\r\n");
    sock.close(Write);

    string buffer;
    while (!(buffer = sock.read()).empty())
    {
        cout << buffer;
    }
    cout << endl;
}
