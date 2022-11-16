#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;



class Timer
{
private:
    //Obs: daca timpul de expirare este mai mare decat aceasta valoare , consideram ca nu expira nicioadata
    long LimMax=5000000000;
public:
    Timer()=default; //definim constructorul implicit

    std::string getCurrentDateTime(std::string s); // returneaza timpul local curent ca STRING
    double getCurrentSec(); // ret timpul local curent ca numar
    std::string getlocalTimeStr(long timeNum); //convertor number --> string

};