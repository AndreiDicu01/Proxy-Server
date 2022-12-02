#include <thread>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <iostream>
#ifndef HTTP_H
#define HTTP_H 1

namespace Utils
{
int ReadClientRequest(int fd, std::string& request);
int ForwardRequest(int sock, std::string& request);

}
#endif
