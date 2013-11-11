// A forking echo server
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;
#include "socket.h"
#include "unistd.h"

ServerSocket create_serverSocket(short port)
{
    try 
    {
        ServerSocket sock(port);
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
    int port = 12321;
    if (argc > 1)
        port = atoi(argv[1]);

    ServerSocket server = create_serverSocket(port);
    cout << "listening on " << port << endl;
    signal(SIGCHLD, SIG_IGN);

    while(true)
    {
        Socket sock = server.accept();
        cout << "New client from: " << sock.remoteHost() << endl;

        if (fork() == 0)
        {
            string buffer;
            while (!(buffer = sock.read()).empty())
            {
                cout << "    " << sock.remoteHost() << ": " << buffer << endl;
                sock.write(buffer);
            }
            cout << "Connection closed: " << sock.remoteHost() << endl;
            exit(0);            
        }
    }
}
