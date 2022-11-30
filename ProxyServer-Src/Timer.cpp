#include "Timer.hpp"
#include <time.h>
std::string Timer::getCurrentDateTime(std::string s)
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);

    if (s == "now")
    {
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    }
    else if (s == "date")
    {
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    }

    return std::string(buf);
}
// copiem in buf prin formatele de mai sus , contentul aflat in tstruct - local time current

double Timer::getCurrentSec()
{
    return time(0);
}
// pur si simplu returnam timpul curent ca numar

std::string Timer::getlocalTimeStr(long timeNum)
{
    if (timeNum > this->LimMax)
    {
        return "Never Expire";
    }
    else
    {
        time_t rawtime = timeNum;
        struct tm *timeinfo;
        timeinfo = localtime(&rawtime);
        std::string timeStr(asctime(timeinfo));
        return timeStr.substr(0, timeStr.size() - 1);
    }
}
//convertor number --> string in cazul in care expira timpul