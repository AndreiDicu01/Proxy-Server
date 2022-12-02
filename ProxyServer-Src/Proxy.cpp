#include "Proxy.hpp"
#include "http-utils.hpp"
#include "Request.hpp"
Proxy* Proxy::m_ProxyInstance=nullptr;

Proxy& Proxy::createInstance(std::string ip,std::string port)
{
    if(m_ProxyInstance!=nullptr)
    {
        //throw;

        
    }

    m_ProxyInstance=new Proxy(ip,port);

    return *m_ProxyInstance;
}

Proxy& Proxy::getInstance()
{
    if(m_ProxyInstance==nullptr)
    {
        //throw;
    
    }

    return *m_ProxyInstance;
}

void Proxy::destroyInstance()
{
    if(m_ProxyInstance==nullptr)
    {
        //throw;
        return;
    }

    delete m_ProxyInstance;
    m_ProxyInstance=nullptr;
}

Proxy::Proxy(std::string ip, std::string port):m_address(ip),m_port(port),m_isSet(false)
{

}

Proxy::~Proxy()
{

}
void Proxy::setListeningSocket()
{
    if(m_isSet == true)
    return;

    int listenSock;
    int checkSockets;
    struct addrinfo address;
    struct addrinfo* list;

    memset(&address,0,sizeof(address));
    address.ai_family=AF_UNSPEC;
    address.ai_socktype=SOCK_STREAM;
    address.ai_flags=AI_PASSIVE;


    checkSockets=getaddrinfo(m_address.c_str(),m_port.c_str(),&address,&list);

    if(checkSockets == -1 )
    {
        //throw
        return;

    }

    listenSock=socket(list->ai_family,list->ai_socktype,list->ai_protocol);

    if(m_listenSocket == -1)
    {
        //throw
        return;
    }

    int yes=1;
    setsockopt(listenSock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
    checkSockets=bind(listenSock,list->ai_addr,list->ai_addrlen);

    if(checkSockets == -1 )
    {
        //throw
        close(listenSock);
        return;
    }

    checkSockets=listen(listenSock,MAX_PENDING_CONNECTIONS);
    if(checkSockets==-1)
        return;
    m_listenSocket=listenSock;
    std::cout<<"\n\n------PROXY-ON-------\n\n";
    std::cout<<"\tSERVER STARTED LISTENING ON PORT: "<<m_port<<std::endl;
    m_isSet=true;
    freeaddrinfo(list);
  
}

int Proxy:: connectToRequestedWeb(std::string host, std::string port)
{
    struct addrinfo addr;
    struct addrinfo* list;
    int webSock;
    int check;

    memset(&addr,0,sizeof(addr));
    addr.ai_family=AF_UNSPEC;
    addr.ai_socktype=SOCK_STREAM;

    check=getaddrinfo(host.c_str(),port.c_str(),&addr,&list);
    if(check!=0)
    {
        fprintf(stderr,"error getting address info");
        return -1;
    }

    webSock=socket(list->ai_family,list->ai_socktype,list->ai_protocol);
    
    if(webSock==-1)
    {
        fprintf(stderr,"error creating web socket\n");
        //
        return -1;
    }

    check=connect(webSock,list->ai_addr,list->ai_addrlen);
    if(check==-1)
    {
        fprintf(stderr,"error connecting to web socket");
        return -1;
    }

    freeaddrinfo(list);
    return webSock;
}

int Proxy:: GET_RequestHandle(Request& req, int clientSock,int serverSock)
{
    if(Utils::ForwardRequest(serverSock,req.getRequest())<0)
        {
            std::cerr<<"Error forwarding request to server\n";
            return -1;
        }

    


}
void Proxy::threadHandle(int clientSocket)
{   
    int check;
    int webSock;
    std::string httpReq;

    check=Utils::ReadClientRequest(clientSocket, httpReq);
    if(check==-1)
        {   close(clientSocket);
            //handle error
            return;
        }
    Request req(httpReq);
    
    std::cout<<"REQUEST:\n";
    std::cout<<req.getRequest()<<std::endl;
    std::cout<<req.getHost()<<std::endl;
    std::cout<<req.getPort()<<std::endl;


    std::string port;
    if(req.getPort().size()==0)
    port="80";   //if no port is specified in the request, the default HTTP port is used
    else
    port=req.getPort();

    webSock=this->connectToRequestedWeb(req.getHost(),port);
    if(webSock==-1)
    {
        fprintf(stderr,"error creating connections socket\n");
        close(clientSocket);
        //error handle
        return;
    }
    

    //redirect requests


}
void Proxy::startHandlingConnections()
{
    int clientSocket;

    struct sockaddr addr;
    socklen_t s_size;

    while(true)
    {
    clientSocket=accept(m_listenSocket, (struct sockaddr *) &addr, &s_size);
    if(clientSocket==-1)
        continue;
    printf("\n __CONN ACCEPTED\n");
 
        /* JUST ONE THREAD FOR NOW*/
        threadHandle(clientSocket);
    }
}