#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;
#include "socket.h"

int main (int argc, char** argv)
{
    int port = 12321;
    if (argc > 1)
        port = atoi(argv[1]);

    try
    {
        ServerSocket server(port);
        cout << "listening on " << port << endl;

        while(true)
        {
            Socket sock = server.accept();
            cout << "New client from: " << sock.remoteHost() << endl;

            string buffer;
            while (!(buffer = sock.read()).empty())
            {
                cout << "Echoing: " << buffer << endl;
                sock.write(buffer);
            }
            cout << "Connection closed: " << sock.remoteHost() << endl;
        }
    }
    catch (exception& e)
    {
        cout << "Error: " << e.what() << endl;
    }
}
