#include <thread>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <iostream>
#ifndef HTTP_H
#define HTTP_H 1

#define CHUNKED_RESPONSE 5

namespace Utils
{
    int ReadClientRequest(int fd, std::string &request);
    int ForwardHTTP(int sock, const std::string &request);
    int SendString(int sock,std::string str);
    int RecieveString(int sock,std::string& str);
    int RecieveResponse(int sock, std::string &resp);
    /***GET***/
    int RecieveUnChunkedResponse(int sock,std::string &resp);
    int RecieveChunked(int client,int server,std::string& resp);

    /***CONNECT***/
    int EstablishHTTPTunnel(int sock);
    int HTTPTunneling(int clientSock,int serverSock);
    namespace ResponseParser
    {
        int getResponseLength(std::string resp);
        int getHeaderLength(std::string& resp);
        int ChunckedResponse(std::string resp);
    }
}
#endif
