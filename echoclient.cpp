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
    string host = "localhost";
    int port = 12321;
    if (argc > 1)
        host = argv[1];
    if (argc > 2)
        port = atoi(argv[2]);

    Socket sock = create_socket(host, port);

    string buffer;
    while (getline(cin, buffer))
    {
        sock.write(buffer);
        cout << "Sending: " << buffer << endl;
        string resp = sock.read();
        cout << "Recieved: " << resp << endl;
    }
    cout << endl;
}

