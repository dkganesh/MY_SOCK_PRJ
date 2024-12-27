#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include<memory>
#include <string>
#include <queue>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define IPV AF_INET
#define PORT htons(9000)
#define CONX_IP INADDR_ANY
#define IP_PROTO SOCK_STREAM

using namespace std;

class Server {
    int serverSocket;
    sockaddr_in serverAddress, clientAddress;
    int serverBind;
    int serverListen;
    int clientConnection;
    static int retry;
    const char* serverMessage = "Server ACK";

    queue<string> storage;
    unique_ptr<char[]> buffer;

    void createSocket() {
        serverSocket = socket(IPV, IP_PROTO, 0);
        if (serverSocket < 0) {
            cout << "Error in Socket creation.\n";
            WSACleanup();
            if (Server::retry > 0) {
                createSocket();
                Server::retry--;
            }
            else {
                cout<<"Server Socket creation failed\n";
                closeSetup();
            }
        }
        Server::retry = 5;
        cout << "Server Socket created: \t" << serverSocket << endl;
    }

    void defineAddress() {
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = PORT;
        serverAddress.sin_addr.s_addr = CONX_IP;
        cout << "Socket Address Defined\n";
    }

    void bindProcess() {
        serverBind = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
        if (serverBind == SOCKET_ERROR) {
            cout << "Bind failed with error code: " << WSAGetLastError() << endl;

            if(Server::retry > 0) {
                bindProcess();
                Server::retry--;
            }
            else {
                cout<<"Server Bind failed\n";
                closeSetup();
            }
        }
        Server::retry = 5;
        cout << "Server Bind Status:\t" << serverBind << endl;
    }

    void listenSocket() {
        serverListen = listen(serverSocket, 5);
        cout << "Server Listen status: \t" << serverListen << endl;
    }

    void acceptClient() {
        int nLen = sizeof(struct sockaddr);
        clientConnection = accept(serverSocket, (struct sockaddr*)&clientAddress, &nLen);
        cout << "Client Accept status:\t" << clientConnection << endl;
    }

    void recvData() {
        buffer = make_unique<char[]>(1024);
        int recvd;
        while (true) {
            recvd = recv(clientConnection, buffer.get(),1024, 0);
            if (recvd > 0) {
                buffer[recvd] = '\0';
                cout << "Message received: " << buffer << endl;
                break;
            } else {
                cout << "Error in receiving data.\n";
                break;
            }
        }
        this_thread::sleep_for(chrono::seconds(3));
        cout<<"sending Acknowledgement to client\n";
        sendAck();
    }

    void sendAck() {
        int sendStatus = send(clientConnection, serverMessage, strlen(serverMessage), 0);
        if (sendStatus == SOCKET_ERROR) {
            cout << "Send failed with error code: " << WSAGetLastError() << endl;
            closeSetup();
        }
        cout<<"Acknowledgement status: "<<sendStatus<<endl;
    }

    void closeSetup() {
        WSACleanup();
        buffer.reset();
        closesocket(serverSocket);
        cout << "Socket Closed\n";
        exit(1);
    }

    void initiateSetup() {
        createSocket();
        defineAddress();
        bindProcess();
        listenSocket();
        acceptClient();
        cout << "Server Setup completed\n" << endl;
        this_thread::sleep_for(chrono::seconds(2));
        recvData();
    }

public:
    Server() {
        thread makeServerReady(&Server::initiateSetup, this);
        makeServerReady.join();
        closeSetup();
    }
};

int Server::retry = 5;

int main() {
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
        std::cout << "WSA failed to initialize" << std::endl;
    } else {
        std::cout << "WSA initialized successfully\n";
    }

    Server server;
}
