CXX=g++
CFLAGS=-Wall -pedantic
LDFLAGS=

## Solaris libraries
#LDFLAGS=-lsocket -lnsl

all: httpclient echoserver echoclient

clean:
	rm -f *.o httpclient echoserver echoclient

socket.cpp.o: socket.cpp socket.h
	$(CXX) $(CXXFLAGS) -c socket.cpp -o socket.cpp.o

httpclient: socket.cpp.o httpclient.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) httpclient.cpp socket.cpp.o -o httpclient

echoserver: socket.cpp.o echoserver.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) echoserver.cpp socket.cpp.o -o echoserver

echoclient: socket.cpp.o echoclient.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) echoclient.cpp socket.cpp.o -o echoclient

