#include "http-utils.hpp"
#include "Proxy.hpp"
#define RECV_BUFF_LENGTH 65536
int Utils::ReadClientRequest(int fd, std::string &request)
{
    int bytesRecieved = 0;
    char temp[2001];
    std::string reqEnd = "\r\n\r\n";
    std::string aux;

    while (true)
    {
        memset(temp, '\0', sizeof(temp));
        bytesRecieved = recv(fd, temp, 2000, (int)NULL);
        if (bytesRecieved == -1)
            return -1;

        aux = temp;
        if (aux.find(reqEnd) != std::string::npos)
        {
            request += aux;
            return true;
        }
        else
            request += aux;
    }
}

int Utils::RecieveUnChunkedResponse(int sock, std::string &resp)
{
    int headerLen;
    int codeLen;
    int totalLen = 0, bytesRead = 0;

    bytesRead = resp.length();
    headerLen = Utils::ResponseParser::getHeaderLength(resp);

    if (headerLen == -1)
        return -1;
    codeLen = Utils::ResponseParser::getResponseLength(resp);
    if (codeLen == -1)
        return -1;

    totalLen = headerLen + codeLen;

    char buff[RECV_BUFF_LENGTH + 1];
    int ret;
    while (bytesRead < totalLen)
    {
        ret = recv(sock, buff, RECV_BUFF_LENGTH, 0);
        if (ret == -1)
        {
            perror("recv");
            return ret;
        }
        bytesRead += ret;
        resp += buff;
        std::memset(buff, 0, RECV_BUFF_LENGTH);
    }

    return bytesRead;
}
int Utils::RecieveChunked(int client, int server, std::string &resp)
{
    if (ForwardHTTP(client, resp) == -1)
        return -1;

    char buff[RECV_BUFF_LENGTH + 1];
    int ret;
    while (true)
    {
        ret = recv(server, buff, RECV_BUFF_LENGTH, 0);
        if (ret == -1)
            return ret;

        if (ret == 0)
            break;

        if (ForwardHTTP(client, buff) == -1)
            return -1;
    }

    return true;
}

int Utils::ResponseParser::getResponseLength(std::string resp)
{
    int poz;
    std::string aux;
    std::string pattern;
    pattern = "Content-Length: ";
    poz = resp.find(pattern.c_str());
    if (poz == std::string::npos)
    {
        std::cerr << "Cant find the length";
        return -1;
    }

    resp = resp.substr(poz + pattern.length());
    aux = resp.substr(0, aux.find("\n"));

    try
    {
        poz = std::stoi(aux);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Bad Content-Length argument for stoi" << '\n';
    }

    return poz;
}

int Utils::ResponseParser::getHeaderLength(std::string &resp)
{
    int poz;
    poz = resp.find("\r\n\r\n");

    if (poz == std::string::npos)
    {
        std::cerr << "Invalid response";
        return -1;
    }

    return poz;
}

int Utils::ForwardHTTP(int sock, const std::string &request)
{
    int len = request.length();
    int bytesSent = 0;
    int ret = 0;
    while (bytesSent < len)
    {
        ret = send(sock, request.c_str() + bytesSent, len - bytesSent, 0);
        if (ret < 0)
        {
            perror("send");
            return ret;
        }
        bytesSent += ret;
    }

    return true;
}
int Utils::SendString(int sock, std::string str)
{
    int len = str.length();
    int bytesSent = 0;
    int ret;

    while (bytesSent < len)
    {
        ret = send(sock, str.c_str() + bytesSent, len - bytesSent, 0);
        if (ret == -1)
        {
            perror("send");
            return -1;
        }
        bytesSent += ret;
    }

    return 1;
}
int Utils::ResponseParser::ChunckedResponse(std::string resp)
{
    if (resp.find("Transfer-Encoding: chunked") != std::string::npos)
        return true;

    return false;
}
int Utils::RecieveString(int sock, std::string &str)
{
    int totalLength = 0, headerLength = 0, contentLength = 0;
    int bytesRecv = 0;
    int aux;
    str = "";
    char buff[RECV_BUFF_LENGTH + 1];

    bytesRecv = recv(sock, buff, RECV_BUFF_LENGTH, 0);
    str += buff;
    if (bytesRecv < 0)
    {
        std::perror("Response recv");
        return -1;
    }

    return 1;
}
int Utils::RecieveResponse(int sock, std::string &resp)
{
    int totalLength = 0, headerLength = 0, contentLength = 0;
    int bytesRecv = 0;
    int aux;
    resp = "";
    char buff[RECV_BUFF_LENGTH + 1];

    bytesRecv = recv(sock, buff, RECV_BUFF_LENGTH, 0);
    resp += buff;
    if (bytesRecv < 0)
    {
        std::perror("Response recv");
        return -1;
    }
    if (Utils::ResponseParser::ChunckedResponse(resp))
        return CHUNKED_RESPONSE;

    return bytesRecv;
}

int Utils::EstablishHTTPTunnel(int sock)
{
    if (Utils::SendString(sock, "HTTP/1.0 200  CONNECTION ESTABLISHED\r\n\r\n") == -1)
    {
        std::cerr << "Error creating HTTP tunnel";
        return -1;
    }

    return 1;
}
int Utils::HTTPTunneling(int clientSock, int serverSock)
{

    int maxSock;
    if (clientSock > serverSock)
        maxSock = clientSock;
    else
        maxSock = serverSock;
    fd_set sockSet_org;
    FD_ZERO(&sockSet_org);
    FD_SET(clientSock, &sockSet_org);
    FD_SET(serverSock, &sockSet_org);

    int ret;
    char buff[RECV_BUFF_LENGTH+1];
    while (true)
    {
        fd_set sockSet_tmp = sockSet_org;
        if (select(maxSock + 1, &sockSet_tmp, NULL, NULL, NULL) == -1)
        {
            std::cerr << "Connect // Select\n";
            return -1;
        }

        for (int i = 0; i <= maxSock; i++)
        {
            if (FD_ISSET(i, &sockSet_tmp))
            {
                if (i == clientSock)
                {
                    
                    //ret = Utils::RecieveResponse(clientSock, buff);
                    memset(buff,0,RECV_BUFF_LENGTH);
                    ret=recv(clientSock,buff,RECV_BUFF_LENGTH,0);
                    if (ret == -1)
                    {
                        std::cerr << "Browser send tunneling error";
                        return -1;
                    }
                    else if (ret == 0)
                        return 1;

                    ret =send(serverSock,buff,ret,0);
                    if (ret == -1)
                    {
                        std::cerr << "Forwarding tunneling error";
                        return -1;
                    }
                }
                else if (i == serverSock)
                {
                    memset(buff,0,RECV_BUFF_LENGTH);   
                    ret=recv(serverSock,buff,RECV_BUFF_LENGTH,0);
                    //ret = Utils::RecieveString(serverSock, buff);
                    if (ret == -1)
                    {
                        std::cerr << "Connect server send ";
                        return -1;
                    }
                    if (ret == 0)
                        return 1;

                    ret=send(clientSock,buff,ret,0);
                    //  ret = Utils::ForwardHTTP(clientSock, buff);
                    if (ret == -1)
                    {
                        std::cerr << "Forwarding back tunneling error";
                        return -1;
                    }
                }
            }
        }
    }

    return 1;
}


