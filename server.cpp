#include<bits/stdc++.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
using namespace std;

int main() {
    //creating socket
    int serverSocket = socket(AF_INET,SOCK_STREAM,0);

    // Specifying the address_information

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Binding the socket and adress info

    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // listen to assinged socket

    listen(serverSocket, 5);
    
    // accepting the connection request

    int clientSocket = accept(serverSocket,nullptr,nullptr);

    // recieving data

    char buffer[1024] = {0};

    recv(clientSocket, buffer, sizeof(buffer), 0);
    cout << "Message from client: " << buffer << endl;

    close(serverSocket);

    return 0;
}