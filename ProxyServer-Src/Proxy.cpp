#include "Proxy.hpp"
#include "http-utils.hpp"
#include "Request.hpp"
#include "Logger.hpp"
Proxy *Proxy::m_ProxyInstance = nullptr;

Proxy &Proxy::createInstance(std::string ip, std::string port)
{
    if (m_ProxyInstance != nullptr)
    {
        // throw;
    }

    m_ProxyInstance = new Proxy(ip, port);

    return *m_ProxyInstance;
}

Proxy &Proxy::getInstance()
{
    if (m_ProxyInstance == nullptr)
    {
        // throw;
    }

    return *m_ProxyInstance;
}

void Proxy::destroyInstance()
{
    if (m_ProxyInstance == nullptr)
    {
        // throw;
        return;
    }

    delete m_ProxyInstance;
    m_ProxyInstance = nullptr;
}

Proxy::Proxy(std::string ip, std::string port) : m_address(ip), m_port(port), m_isSet(false)
{
}

Proxy::~Proxy()
{
}
void Proxy::setListeningSocket()
{
    if (m_isSet == true)
        return;

    int listenSock;
    int checkSockets;
    struct addrinfo address;
    struct addrinfo *list;

    memset(&address, 0, sizeof(address));
    address.ai_family = AF_UNSPEC;
    address.ai_socktype = SOCK_STREAM;
    address.ai_flags = AI_PASSIVE;

    checkSockets = getaddrinfo(m_address.c_str(), m_port.c_str(), &address, &list);

    if (checkSockets == -1)
    {
        // throw
        return;
    }

    listenSock = socket(list->ai_family, list->ai_socktype, list->ai_protocol);

    if (m_listenSocket == -1)
    {
        // throw
        return;
    }

    int yes = 1;
    setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    checkSockets = bind(listenSock, list->ai_addr, list->ai_addrlen);

    if (checkSockets == -1)
    {
        // throw
        close(listenSock);
        return;
    }

    checkSockets = listen(listenSock, MAX_PENDING_CONNECTIONS);
    if (checkSockets == -1)
        return;
    m_listenSocket = listenSock;
    std::cout << "\n\n------PROXY-ON-------\n\n";
    std::cout << "\tSERVER STARTED LISTENING ON PORT: " << m_port << std::endl;
    m_isSet = true;
    freeaddrinfo(list);
}

int Proxy::connectToRequestedWeb(std::string host, std::string port)
{
    struct addrinfo addr;
    struct addrinfo *list;
    int webSock;
    int check;

    memset(&addr, 0, sizeof(addr));
    addr.ai_family = AF_UNSPEC;
    addr.ai_socktype = SOCK_STREAM;

    check = getaddrinfo(host.c_str(), port.c_str(), &addr, &list);
    if (check != 0)
    {
        fprintf(stderr, "error getting address info");
        return -1;
    }

    webSock = socket(list->ai_family, list->ai_socktype, list->ai_protocol);

    if (webSock == -1)
    {
        fprintf(stderr, "error creating web socket\n");
        //
        return -1;
    }

    check = connect(webSock, list->ai_addr, list->ai_addrlen);
    if (check == -1)
    {
        fprintf(stderr, "error connecting to web socket");
        return -1;
    }

    freeaddrinfo(list);
    return webSock;
}
int Proxy::CONNECT_RequestHandle(int clientSock,int serverSock,int idCon)
{
  if(Utils::EstablishHTTPTunnel(clientSock)==-1)
    return -1;

    if(Utils::HTTPTunneling(clientSock,serverSock)==-1)
    return -1;


    return 1;


}

int Proxy::GET_RequestHandle(Request &req, int clientSock, int serverSock,int idCon,std::string host)
{
    int check;
    if (Utils::ForwardHTTP(serverSock, req.getRequest()) < 0)
    {
        std::cerr << "Error forwarding request to server\n";
        return -1;
    }

    std::string response;
    check = Utils::RecieveResponse(serverSock, response);
    
    if (check == -1)
    {
        std::cerr << "Error recieving response from server";
        return -1;
    }
    else if (check == CHUNKED_RESPONSE)
    {   
          
        if (Utils::RecieveChunked(clientSock, serverSock, response) == -1)
            return -1;
        close(serverSock);
       //  Logger::logResponse(idCon,response,host);
    }
    else
    {     
        if (Utils::RecieveUnChunkedResponse(serverSock, response) == -1)
        {
            std::cerr << "Error recieving unchunked response";
            return -1;
        }
        close(serverSock);
        if (Utils::ForwardHTTP(clientSock, response) == -1)
        {
            std::cerr << "Error sending unchunked";
            return -1;
        }
        // Logger::logResponse(idCon,response,host);
    }
   
    return 1;
}
void threadHandle(Proxy& proxy,int clientSocket,int conId)
{
    std::cout<<"\nTHREAD HANDLE";
    int check;
    int webSock;
    std::string httpReq;

    if(check = Utils::ReadClientRequest(clientSocket, httpReq)==-1)
        return;
    if (check == -1)
    {
        close(clientSocket);
        // handle error
        return;
    }
   
    Request req(httpReq);
    Logger::logRequest(conId,req);
    std::string port;
    if (req.getPort().size() == 0)
        port = "80"; // if no port is specified in the request, the default HTTP port is used
    else
        port = req.getPort();

    std::cout << "\n-----------------------------------------------\n"
              << "Recieved request: \n"
              << req.getStatusLine()
              << "\n-----------------------------------------------\n";
    webSock = proxy.connectToRequestedWeb(req.getHost(), port);
    if (webSock == -1)
    {
        fprintf(stderr, "error creating connections socket\n");
        close(clientSocket);
        // error handle
        return;
    }

    Type header=req.getReqType();
    switch(header)
    {
        case Type::GET:
            proxy.GET_RequestHandle(req, clientSocket, webSock,conId,req.getHost());
            break;
        case Type::CONNECT:
            proxy.CONNECT_RequestHandle(clientSocket,webSock,conId);
            break;
        case Type::POST:
            proxy.GET_RequestHandle(req,clientSocket,webSock,conId,req.getHost());
            break;
        default:
            std::cerr<<"Unknown request type";
        break;
    }
    
    close(clientSocket);
    close(webSock);
    // redirect requests
}
void Proxy::startHandlingConnections()
{
    int clientSocket;

    struct sockaddr addr;
    socklen_t s_size;
    int conId=1;
    while (true)
    {
        clientSocket = accept(m_listenSocket, (struct sockaddr *)&addr, &s_size);
        if (clientSocket == -1)
            continue;
        printf("\n __CONN ACCEPTED\n");

        /*JUST ONE THREAD FOR NOW*/
        conId++;
        std::thread(threadHandle,std::ref(*this),clientSocket,conId).detach();

      
        
    }
    close(clientSocket);

}