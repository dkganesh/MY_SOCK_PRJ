#include "WSA_Configs.h"


WSA_Configs::WSA_Configs(){
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
		std::cout << "WSA failed to initialize" << std::endl;
	}
	else {
		std::cout << "WSA initialized successfully...\n";
	}
}