#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include <string>
#include <memory>
#include <queue>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define IPV AF_INET
#define C_PORT htons(7000)
#define S_PORT htons(9000)
#define CONX_IP INADDR_ANY
#define IP_PROTO SOCK_STREAM

using namespace std;

class Client {
    int clientSocket,usr_port;
    sockaddr_in clientAddress, serverAddress;
    unique_ptr<char []> buffer;
    char r_buffer[1024];
    unique_ptr<const char*>ipArray;
    queue<string> c_storage;
    static int retry;
    int clientBind;

    void getMessage() {
        buffer = make_unique<char[]>(1024); 
        cout << "Enter message: \t";
        cin.getline(buffer.get(), 1024);  
    }

    const char* getIP() {
            static string ip;
            cout << "Enter Server IP: ";
            getline(cin, ip);
            ipArray = make_unique<const char*>(ip.c_str());
            cout << "Connecting to server at IP: " << *ipArray << endl;
            return ip.c_str();
        }

    void createSocket() {
        clientSocket = socket(IPV, IP_PROTO, 0);
        if (clientSocket < 0) {
            cout << "Error in Socket creation.\n";
            WSACleanup();
            if (Client::retry > 0) {
                createSocket();
                Client::retry--;
            }
        }
        Client::retry = 5;
        cout << "Client Socket created: \t" << clientSocket << endl;
    }

    void defineSocket() {
        
        getIP();
        clientAddress.sin_family = IPV;
        clientAddress.sin_port = htons(usr_port);
        clientAddress.sin_addr.s_addr = INADDR_ANY;

        serverAddress.sin_family = IPV;
        serverAddress.sin_port = S_PORT;
        serverAddress.sin_addr.s_addr = inet_addr(*ipArray);
    }

    void connectServer() {
        clientBind = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        if (clientBind < 0) {
            cout << "Failed to connect to server.\n";
            if (Client::retry > 0) {
                Client::retry--;
                defineSocket();
                connectServer();
                
            }
            else {
                cout<<"Client Connection failed\n";
                closeSetup();
            }
        }
        cout << "Client connected to server.\n\n";
    }
    void recvAck() {
        int recvd;
        cout<<"Waiting for server Acknowledgement\n";
        while (true) {
            recvd = recv(clientSocket, r_buffer, sizeof(r_buffer), 0);
            if (recvd > 0) {
                r_buffer[recvd] = '\0';
                cout << "Server Message : " << r_buffer << endl;
                break;
            } else {
                cout<<"Client Recvd failed with Error code: "<<WSAGetLastError()<<endl;
                break;
            }
            this_thread::sleep_for(chrono::seconds(1));
        }
    }
    void sendData() {
        getMessage();
        int sendStatus = send(clientSocket, buffer.get(), strlen(buffer.get()), 0);
        if (sendStatus == SOCKET_ERROR) {
            cout << "Send failed with error code: " << WSAGetLastError() << endl;
            WSACleanup();
            exit(1);
        }
        cout << "Data sent to server: \t" << sendStatus << endl;
        this_thread::sleep_for(chrono::seconds(2));
        recvAck();
    }

    void initiateSetup() {
        createSocket();
        defineSocket();
        connectServer();
        this_thread::sleep_for(chrono::seconds(2));
        sendData();
    }

    void closeSetup() {
        closesocket(clientSocket);
        ipArray.reset();
        buffer.reset();
        WSACleanup();
        cout << "Socket Closed\n";
        exit(0);
    }

public:
    Client() {   
        cout<<"Enter Client Port Number: \t";
        cin>>usr_port;
        cin.ignore();

        buffer = nullptr;

        thread makeClientReady(&Client::initiateSetup, this);
        makeClientReady.join();
        closeSetup();
    }
};

int Client::retry = 5;

int main() {
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
        std::cout << "WSA failed to initialize" << std::endl;
    } else {
        std::cout << "WSA initialized successfully\n\n";
    }
    Client client;
}
