#include <iostream> 
#include <WinSock2.h> 
#include <Windows.h>
#include <WS2tcpip.h>
#include <stdlib.h> 
#include <string.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std; 
class NNSocket
{
private:
    WSADATA ws;
    int iResult;
    SOCKET sock;
    SOCKET client;
public:
    NNSocket(/* args */);
    ~NNSocket();
    int initServer();
    char* recvMessage();
    bool sendMessage(char * );
    int shutdownServer();
};

NNSocket::NNSocket(/* args */)
{
    sock = INVALID_SOCKET;
    client = INVALID_SOCKET;
}

NNSocket::~NNSocket()
{
}
/**
 * Starts Socket server at port 10020
 * @param returnvar: Non zero return value states error while starting server
 */
int NNSocket::initServer(){
    //Init winsock
    iResult = WSAStartup(MAKEWORD(2,2),&ws);
    if(iResult != 0){
        cout<<"Error in Winsock init. Function returned "<<iResult;
        return 0;
    }

    struct sockaddr_in s1;
    hostent *localhost;
    
    //Creating Socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock == INVALID_SOCKET){
        cout<<"Error creating socket... Ending Program, "<<WSAGetLastError();
        WSACleanup();
        return 0;
    }

    localhost = gethostbyname("");
    char *localip = inet_ntoa(*(struct in_addr*)* localhost->h_addr_list);
    s1.sin_addr.s_addr = htonl(INADDR_ANY);
    s1.sin_family = AF_INET;
    s1.sin_port = htons(10020);

    iResult = bind(sock,(SOCKADDR*)&s1, sizeof(s1));
    if(iResult != 0){
        cout<<"Error binding socket... Ending Program, "<<WSAGetLastError()<<endl;
        WSACleanup();
        return 0;
    }
    if(listen(sock,SOMAXCONN) == SOCKET_ERROR){
        cout<<"Server listen error, "<<WSAGetLastError();
        closesocket(sock);
        WSACleanup();
        return 0;
    }else{
        cout<<"Server is listening... ";
    }
    return 0;
}
/**
 * Recieves Message sent by the client
 * @param returnvar: Contains Message sent by the client. If NULL the client has stopped sending messages or error has occured.
 *  
 */
char* NNSocket::recvMessage(){

    //Client Connection
    client = accept(sock, NULL, NULL);
    if(client == INVALID_SOCKET){
        cout<<"No connection accepted, "<<WSAGetLastError();
        closesocket(sock);
        WSACleanup();
        return 0;
    }

    char recvbuf[1024];
    int recvLength = 1024;

    iResult = recv(client, recvbuf, recvLength, 0);
    if(iResult > 0){
        return recvbuf;
    }else if(iResult <= 0){
        cout<<"iResult <= 0"<<endl;
        closesocket(client);
        WSACleanup();
        return NULL;
    }
}
/**
 * Send message to socket client
 * @param msg: Message to be sent. msg = NULL if no more message is to be sent 
 * @param returnvar: True if sending is successful else false. Check logs for error codes.
 */
bool NNSocket::sendMessage(char* msg){
    iResult = send(client, msg, (int)strlen(msg), 0);
    if(iResult == SOCKET_ERROR ){
        cout<<"Sending message failed: "<<iResult;
        closesocket(client);
        WSACleanup();
        return false;
    }else{
        return true;
    }
}
/**
 * Close socket server and disconnect from client
 */
int NNSocket::shutdownServer(){
    iResult = shutdown(client, SD_SEND);
    if(iResult == SOCKET_ERROR){
        cout<<"Shutdown Failed, Error: "<<WSAGetLastError();
        WSACleanup();
        return 0;
    }
    closesocket(client);
    WSACleanup();
    return 0;
}