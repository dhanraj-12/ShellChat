#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>  // For inet_addr
#include <unistd.h>
using namespace std;

int main()
{
    // Creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return -1;
    }

    // Specifying server address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    
    // Replace with actual server IP address, e.g., 192.168.1.5
    serverAddress.sin_addr.s_addr = inet_addr("192.168.x.x");  // Replace x.x with the server's actual IP

    // Sending connection request
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Connection failed." << std::endl;
        close(clientSocket);
        return -1;
    }

    // Sending data
    // const char* message = "Hello, server!";
    string message;
    cout << "enter the message" << endl;
    getline(cin,message);

    send(clientSocket, message.c_str(), message.size(), 0);

    std::cout << "Message sent to server." << std::endl;

    // Closing socket
    close(clientSocket);

    return 0;
}
