#include<bits/stdc++.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>  // For inet_addr
#include <unistd.h>
#include<thread>
using namespace std;

string CeaserEncrypt(const string& message,int shift) {
    string result = message;
    for(int i = 0; i<message.size(); i++) {
        if(isalpha(result[i])) {
            char base = islower(result[i]) ? 'a':'A';
            result[i] = (result[i]-base+shift)%26+base;
        }

    }

    return result;
}   

string CeaserDecrypt(const string& message,int shift) {
    string result = message;
    for(int i = 0; i<message.size(); i++) {
        if(isalpha(result[i])) {
            char base = islower(result[i]) ? 'a':'A';
            result[i] = (result[i]-base-shift+26)%26+base;
        }

    }

    return result;
}  

string shift_to_binary(int shift) {
    return bitset<4>(shift).to_string();
}

int binary_to_shift(const string& binary) {
    return bitset<4>(binary).to_ulong();
}


void sendmessage(int clienSocket) {
    cout << "Enter your Chatname" << endl;
    string name;
    getline(cin, name);
    string message;                                                                                                                                            
    while(1) {
        getline(cin, message);
        string msg = name + ":" + message;
        // seeding rand function using time
        srand(time(0));
        int shift = rand() % 12 + 1; 
        // encrypting message
        string encryptmsg = CeaserEncrypt(msg,shift); 
        // converting shift to binary_string
        string binaryshift = shift_to_binary(shift);
        // passing the decrypting key with the message;
        string msgtosend = binaryshift+encryptmsg;
        int bytesend = send(clienSocket,msgtosend.c_str(),msgtosend.size(),0);
        if(bytesend == -1) {
            cerr << "error while sending the application" << endl;
            break;
        }

        if(message == "quit") {
            cout << "stopping the application..." << endl;
            close(clienSocket);
            break;  
        }
    }

    close(clienSocket);

}

void recievemessage(int clientSocket) {

    char buffer[1024] = {0};
    ssize_t recvlen;
    string msg;
    while(1) {
        recvlen = recv(clientSocket,buffer,sizeof(buffer),0);
        if(recvlen <= 0) {
            cerr << "dissconnect from the server" << endl;
            break;
        }else {
            msg = string(buffer,recvlen);
            // extracting binary string 
            string binarystring  = msg.substr(0,4);
            // converting the string string to key or sift;
            int key = binary_to_shift(binarystring);
            // extracting the encrypted msg
            string encryptedmsg = msg.substr(4);
            // decrypting the msg
            string decryptedmsg = CeaserDecrypt(encryptedmsg,key);

            cout << decryptedmsg << endl;

        }

        
    }
    close(clientSocket);

}



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
    // serverAddress.sin_addr.s_addr = inet_addr("35.154.242.252");  // Replace x.x with the server's actual IP

    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Sending connection request
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Connection failed." << std::endl;
        close(clientSocket);
        return -1;
    }else {
        cout << "Sucessfully connected to server" << endl; 
    }


    thread senderthread(sendmessage,clientSocket);
    thread recieverthread(recievemessage,clientSocket);

    senderthread.join();
    recieverthread.join();


    return 0;
}
