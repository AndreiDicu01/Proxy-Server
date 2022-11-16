#include "http-utils.hpp"
#include "Proxy.hpp"
int Utils::ReadClientRequest(int fd, std::string& request)
{
    int bytesRecieved=0;
    char temp[2001];
    std::string reqEnd="\r\n\r\n";
    std::string aux;

    while(true)
    {   
        memset(temp,'\0',sizeof(temp));
        bytesRecieved=recv(fd,temp,2000,(int)NULL);
        if( bytesRecieved == -1)
        return false;

        aux=temp;
        if(aux.find(reqEnd)!=std::string::npos)
        {
                request+=aux;
                return true;
        }
        else
        request+=aux;

    }
}
