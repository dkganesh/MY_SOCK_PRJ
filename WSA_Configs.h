#include <iostream>
#include<cstring>
#include<string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class WSA_Configs{

private:
    WSADATA ws;
public:
    WSA_Configs();
};