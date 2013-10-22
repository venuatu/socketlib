#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;
#include "socket.h"

int main (int argc, char** argv)
{
    string host = "www.google.com";
    int port = 12321;
    if (argc > 1)
        host = argv[1];
    if (argc > 2)
        port = atoi(argv[2]);

    try
    {
        Socket sock(host, port);

        string buffer;
        while (getline(cin, buffer))
        {
            sock.write(buffer);
            cout << "Sending: " << buffer << endl;
            string resp = sock.read();
            cout << "Recieved: " << resp << endl;
        }
    }
    catch (exception& e)
    {
        cout << "Error: " << e.what() << endl;
    }
    cout << endl;
}

