#include "NNSocket.hxx"
#include <Windows.h>
int main(){
    NNSocket server;
    server.initServer();
    while(true){
        char* msg = server.recvMessage();
        cout<<msg<<endl;
        server.sendMessage(msg);
    }
    return 0;
}