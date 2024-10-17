#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring> // For memset
#include<thread>

using namespace std;

void interactWithClient(int clientSocket,vector<int>& clients) {
    char buffer[1024] = {0};

    while(1) {
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cerr << "Failed to receive data and client disconnected" << endl;
            close(clientSocket); 
            break;
        }
        string message(buffer,bytesReceived);
        cout << "Message from client: " << message << endl;

        for(auto client : clients) {
            if(client != clientSocket) {
                send(client,message.c_str(), message.length(), 0);
            }
            
        }
    }
    
    // removing the socket which is closed
    auto it = find(clients.begin(), clients.end(), clientSocket);
    if(it != clients.end()) {
        clients.erase(it);
    }
    // Closing sockets
    close(clientSocket);
}

int main() {
    // Creating socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Failed to create socket." << endl;
        return -1;
    }

    // Specifying the address information
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces

    // Binding the socket to the address
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        cerr << "Bind failed." << endl;
        close(serverSocket);
        return -1;
    }

    // Listening on the assigned socket
    if (listen(serverSocket, 5) == -1) {
        cerr << "Listen failed." << endl;
        close(serverSocket);
        return -1;
    }

    cout << "Server is listening on port 8080..." << endl;

    // Accepting the connection request

    vector<int> clients;

    while(1) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            cerr << "Failed to accept connection." << endl;
            close(serverSocket);
            return -1;
        }else {
            cout << "Client is connected" << endl;
        }

        clients.push_back(clientSocket);

        thread t1(interactWithClient,clientSocket,ref(clients));
        t1.detach();
    }
    
    close(serverSocket);

    return 0;
}
