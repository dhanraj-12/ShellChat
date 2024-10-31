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
   // Ensure the shift value is within valid range (0-15)
    shift = shift & 0xF;  // Mask to 4 bits
    return bitset<4>(shift).to_string();
}

int binary_to_shift(const string& binary) {
    // Validate binary string length and content
    if (binary.length() != 4) {
        throw runtime_error("Invalid binary string length");
    }
    for (char c : binary) {
        if (c != '0' && c != '1') {
            throw runtime_error("Invalid binary character");
        }
    }
    return bitset<4>(binary).to_ulong();
}

bool inChatMode = false;
string username;
mutex cout_mutex;
bool waitingForUsername = false;


void sendmessage(int clienSocket) {
     string message;
    
    // Wait for initial system messages
    this_thread::sleep_for(chrono::milliseconds(500));                                                                                                                                           
    while(1) {
        
        if(username.empty() && inChatMode) {
            {
                lock_guard<mutex> lock(cout_mutex);
                waitingForUsername = true;
                cout << "\033[1;32mEnter your username: \033[0m";
            }
            
            getline(cin, username);
            
            {
                lock_guard<mutex> lock(cout_mutex);
                waitingForUsername = false;
                while(username.empty()) {
                    cout << "\033[1;31mUsername cannot be empty. Please enter your username: \033[0m";
                    getline(cin, username);
                }
                cout << "\033[1;32mWelcome, " << username << "! You can now start sending messages.\033[0m" << endl;
            }
            continue;
        }


        getline(cin, message);

        if(message == "quit") {
            cout << "Stopping the application..." << endl;
            close(clienSocket);
            break;
        }


        string msgtosend;
        if(!inChatMode) {
            msgtosend = message;
        } else {
            string msg = username + ": " + message;
            srand(time(0));
            int shift = rand() % 12 + 1;
            string encryptmsg = CeaserEncrypt(msg, shift);
            string binaryshift = shift_to_binary(shift);
            msgtosend = "MSG:" + binaryshift + encryptmsg;
        }

         
        int bytesend = send(clienSocket, msgtosend.c_str(), msgtosend.size(), 0);
        if(bytesend == -1) {
            cerr << "Error while sending the message" << endl;
            break;
        }
    }

    close(clienSocket);

}

void recievemessage(int clientSocket) {

    char buffer[1024] = {0};
    ssize_t recvlen;

    while(1) {
        recvlen = recv(clientSocket,buffer,sizeof(buffer),0);
        if(recvlen <= 0) {
            cerr << "dissconnect from the server" << endl;
            break;
        }
        string msg(buffer, recvlen);
        memset(buffer, 0, sizeof(buffer));

         if(msg.substr(0, 4) == "SYS:") {
            cout << "\033[1;33m" << msg.substr(4) << "\033[0m" << endl;
            
            if(msg.find("Successfully joined") != string::npos || 
               msg.find("Created new public room") != string::npos ||
               msg.find("Joined existing room") != string::npos) {
                inChatMode = true;
                cout << "\033[1;32m" << "Entered chat mode - messages will now be encrypted" << "\033[0m" << endl;
                
              
            }
        } else if(msg.substr(0, 4) == "MSG:" && msg.length() >= 8) {
            try {
                string binaryshift = msg.substr(4, 4);  // Changed from 8 to 4
                int key = binary_to_shift(binaryshift);
                string encryptedmsg = msg.substr(8);
                string decryptedmsg = CeaserDecrypt(encryptedmsg, key);
                cout << "\033[1;37m" << decryptedmsg << "\033[0m" << endl;
            } catch(const exception& e) {
                cerr << "Error decrypting message: " << e.what() << endl;
                // Print debug information
                cout << "Debug - Full message: " << msg << endl;
                cout << "Debug - Message length: " << msg.length() << endl;
                if (msg.length() >= 8) {
                    cout << "Debug - Binary portion: " << msg.substr(4, 4) << endl;
                }
            }
        } else {
            cout << msg << endl;
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
