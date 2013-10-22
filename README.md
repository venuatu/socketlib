# socketlib

This is a pretty simple socket library that wraps the BSD TCP socket API and provides deterministic destruction and a simple read and write API based around std::string

The current tested targets are:

-   Linux: gcc 4.6.3, gcc 4.7.3, clang 3.0, clang 3.2
-   Mac OSX: llvm-gcc 4.2.1, gcc 4.8.1, apple clang 4.2 (3.2svn) 
-   Solaris 10 SPARC: gcc 4.2.2, CC v12.3

Caveats:

-   Will only work on systems that directly provide the BSD APIs (i.e. no windows support (including cygwin))
-   If you don't ignore SIGPIPE on each thread it will interrupt and kill your program and the EPIPE errors are raised by an exception on whichever method was called and it's easier to handle these exceptions than dealing directly with SIGPIPE
    -   The library tries to do this when the Socket class is created and copied but otherwise needs to be done manually

## Why?

If you want a full featured and / or battle hardened C++ socket wrapper see: [Boost.ASIO](http://www.boost.org/doc/libs/1_54_0/doc/html/boost_asio.html), [QT network](http://qt-project.org/doc/qt-5.1/qtdoc/topics-network-connectivity.html) or find another one on http://lmgtfy.com/?q=c%2B%2B+socket+library

The main aspect of this library is providing a thin wrapper that doesn't require megabytes of headers (not even close).

## License

Either public domain or UNLICENSE, whichever is better for you.

## Examples

See:

-   [[httpclient.cpp]]
-   [[echoserver.cpp]]
-   [[echoclient.cpp]]

## TODO

-   Migrate away from the hand rolled reference counting to shared_ptr
-   Use clearer exception classes for each error or find another way
-   Provide UDP support

