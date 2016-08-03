# NetLib

C++ framework to simplify the client-server routine via sockets.

##Content

1. Server     - async web server to handle incoming connections and receive/send data
2. Client     - async web client to connect to the remote server and send/receive data
3. HttpServer - async HTTP web server to handle HTTP requests
4. HttpClient - not implemented yet

##Usage

Usage is shown in the sample projects and is simple as:

Server server();

server.start(1234);

<br>

Anton Egorov, 2016
dohxehapo@gmail.com
