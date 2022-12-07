#include "http-utils.hpp"
#include "Proxy.hpp"
#define RECV_BUFF_LENGTH 65536
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
        return -1;

        
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

int  Utils::RecieveUnChunkedResponse(int sock,std::string &resp)
{
    int headerLen;
    int codeLen;
    int totalLen=0, bytesRead=0;

    
    bytesRead=resp.length();
    headerLen=Utils::ResponseParser::getHeaderLength(resp);
    if(headerLen==-1)
        return -1;
    codeLen=Utils::ResponseParser::getResponseLength(resp);
    if(codeLen==-1)
        return -1;

    totalLen=headerLen+codeLen;


    char buff[RECV_BUFF_LENGTH];
    int ret;
    while(bytesRead<totalLen)
    {
        ret=recv(sock,buff,RECV_BUFF_LENGTH,0);
        if(ret==-1)
            return ret;

        bytesRead+=ret;
        resp+=buff;
    }

    return bytesRead;

}
int Utils::RecieveChunked(int client,int server,std::string& resp)
{
    if(ForwardHTTP(client,resp)==-1)
        return -1;

    char buff[RECV_BUFF_LENGTH];
    int ret;
    while(true)
    {
        ret=recv(server,buff,RECV_BUFF_LENGTH,0);
        if(ret==-1)
            return ret;

        if(ret==0)
            break;

        if(ForwardHTTP(client,buff)==-1)
            return -1;
    }

    return true;
}

int Utils::ResponseParser::getResponseLength(std::string resp)
{
    int poz;
    std::string aux;
    std::string pattern;
    pattern="Content-Length: ";
    poz=resp.find(pattern.c_str());
    if(poz==std::string::npos)
        {
            std::cerr<<"Cant find the length";
            return -1;
        }
    
    resp=resp.substr(poz+pattern.length());
    aux=resp.substr(0,aux.find("\n"));

    try
    {
        poz=std::stoi(aux);   
    }
    catch(const std::exception& e)
    {
        std::cerr << "Bad Content-Length argument for stoi"<< '\n';
    }
    
    return poz;
}


int Utils::ResponseParser:: getHeaderLength(std::string& resp)
{
    int poz;
    poz=resp.find("\r\n\r\n");

    if(poz==std::string::npos)
    {
        std::cerr<<"Invalid response";
        return -1;
    }

    return poz;
}


int Utils::ForwardHTTP(int sock,const std::string& request)
{
    int len=request.length();
    int bytesSent=0;
    int ret=0;
    while(bytesSent<len)
    {
        ret=send(sock,request.c_str()+bytesSent,len-bytesSent,0);
        if(ret<0)
        return ret;

        bytesSent+=ret;
    }

    return true;

}
int Utils::ResponseParser::ChunckedResponse(std::string resp)
{
    if(resp.find("Transfer encoding: chunked")!=std::string::npos)
        return true;

    return false;
}
int Utils::RecieveResponse(int sock, std::string& resp)
{
    int totalLength=0,headerLength=0,contentLength=0;
    int bytesRecv=0;
    int aux;
    resp="";
    char buff[RECV_BUFF_LENGTH];

    bytesRecv=recv(sock,buff,RECV_BUFF_LENGTH,0);
    resp+=buff;
    if(bytesRecv<0)
    {
        std::perror("Response recv");
        return -1;
    }
   if(Utils::ResponseParser::ChunckedResponse(resp))
    return CHUNKED_RESPONSE;
    
    return bytesRecv;
}

