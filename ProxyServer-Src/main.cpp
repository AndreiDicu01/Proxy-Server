#include "Proxy.hpp"

int main()
{   
    std::string serverIP="0.0.0.0";
    std::string port="12345";
    try{
    Proxy& httpProxy=Proxy::createInstance(serverIP,port);
    httpProxy.setListeningSocket();
    httpProxy.startHandlingConnections(); 
    
    }
    catch(...)
    {

    }
    return 0;

}