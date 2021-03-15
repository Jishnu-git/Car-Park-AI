#include "NNSocket.hxx"
int main(){
    NNSocket server;

    server.initServer();
    char* msg = server.recvMessage();
    cout<<msg<<endl;
    server.sendMessage(msg);
    return 0;
}