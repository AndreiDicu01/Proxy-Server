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
    int RecieveResponse(int sock, std::string &resp);
    int RecieveUnChunkedResponse(int sock,std::string &resp);
    int RecieveChunked(int client,int server,std::string& resp);
    namespace ResponseParser
    {
        int getResponseLength(std::string resp);
        int getHeaderLength(std::string& resp);
        int ChunckedResponse(std::string resp);
    }
}
#endif
