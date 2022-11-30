#include <stdio.h>
#include <string>

enum class Type
{ GET,POST,CONNECT,INVALID};
class Request
{   
    private:
    /*header items*/          
    std::string m_host;
    std::string m_port;
        /**/
    Type m_reqType;
    std::string m_req;  /*first line request*/

    private:
    void parse(std::string str);
    public:
    Request(std::string catched);
    std::string getHost();
    std::string getPort();
    std::string getRequest();
    Type getReqType();


};