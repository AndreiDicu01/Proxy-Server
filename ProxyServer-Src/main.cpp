#include "Proxy.hpp"
#include "Cache.hpp"
int main()
{
    std::string serverIP = "0.0.0.0";
    std::string port = "12345";
    try
    {
        Proxy &httpProxy = Proxy::createInstance(serverIP, port);
       // WebCache& cache=WebCache::createInstance(100);
        httpProxy.setListeningSocket();
        httpProxy.startHandlingConnections();
    }
    catch (...)
    {
    }
    return 0;
}