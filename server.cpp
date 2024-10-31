#include <bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring> // For memset
#include<thread>

using namespace std;

mutex roomMutex;

struct Room {
    string name;
    vector<int> clients;
    bool isprivate;
    string key; // key for private room
};

// Map to strore all chat Room
map<string,Room> chatRooms;

// New Function for System Message

void sendSysteMessage(int clientSocket, const string& message) {
     // Add a special prefix "SYS:" to mark system messages
    string systemMessage = "SYS:" + message;
    send(clientSocket, systemMessage.c_str(), systemMessage.size(), 0);
} 

void sendMessage(const string& roomName, const string& messages, int sendersocket) {
    lock_guard<mutex> lock(roomMutex);
    auto& room = chatRooms[roomName];
    for(int client: room.clients) {
        if(client != sendersocket) {
            // don't send the message back to sender
            send(client,messages.c_str(), messages.size(),0);
        }
    }
}

void interactWithClient(int clientSocket) {
    char buffer[1024] = {0};
    string roomname;

    // Ask if user want to selectt public or private
    string menu = "Welcome to the chat server! Choose an option: \n1. Public\n2. Private";
    sendSysteMessage(clientSocket,menu);

    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
        cerr << "Failed to receive data and client disconnected" << endl;
        close(clientSocket); 
        return;
    }
    
    string choice(buffer,bytesReceived);
    memset(buffer,0,sizeof(buffer));

    if(choice == "1") {
        // Handle public chat rroom
        string publicMenu = "SERVER: Available public chat room:\n";
        // List available public rrooms

        {
            lock_guard<mutex> lock(roomMutex);
            for(const auto& [roomname,Room] : chatRooms) {
                if(!Room.isprivate) {
                    publicMenu += "- "+ roomname + "\n";
                }
            }
        }

        publicMenu += "Enter the name of the public chat room you want to join (or create a new one):\n";
        sendSysteMessage(clientSocket,publicMenu);

        bytesReceived = recv(clientSocket,buffer,sizeof(buffer),0);
        if (bytesReceived <= 0) {
            cerr << "Failed to receive data and client disconnected" << endl;
            close(clientSocket); 
            return;
        }

        roomname = string(buffer,bytesReceived);
        memset(buffer,0,sizeof(buffer));


        // Add a client to a choosen public room 
        {
           lock_guard<mutex> lock(roomMutex);
            if(chatRooms.find(roomname) == chatRooms.end()) {
                chatRooms[roomname] = Room{roomname, {}, false, ""};
                sendSysteMessage(clientSocket, "Created new public room: " + roomname);
            } else {
                sendSysteMessage(clientSocket, "Joined existing room: " + roomname);
            }
            chatRooms[roomname].clients.push_back(clientSocket);
        }

    } else if(choice == "2") {
        // handling the part of private chat room
        string privateMenu = "Private chat option:\n1. create a private room\n2. Join a private room\n";
        sendSysteMessage(clientSocket,privateMenu);

        bytesReceived = recv(clientSocket,buffer,sizeof(buffer),0);
        if (bytesReceived <= 0) {
            cerr << "Failed to receive data and client disconnected" << endl;
            close(clientSocket); 
            return;
        }

        string privateChoice(buffer,bytesReceived);
        memset(buffer,0,sizeof(buffer));

        if(privateChoice == "1") {
            string roomcreation = "Enter the name of private chatroom:\n";
            sendSysteMessage(clientSocket, roomcreation);
            
            bytesReceived = recv(clientSocket,buffer,sizeof(buffer),0);
            if (bytesReceived <= 0) {
                cerr << "Failed to receive data and client disconnected" << endl;
                close(clientSocket); 
                return;
            }

            roomname = string(buffer,bytesReceived);
            memset(buffer,0,sizeof(buffer));

            // Generating the key for the private room
            srand(time(0));
            string key = to_string(rand() % 10000); 
            {
                lock_guard<mutex> lock(roomMutex);
                chatRooms[roomname] = Room{roomname,{},true,key};
                chatRooms[roomname].clients.push_back(clientSocket);
            }

            string keyMessage = "private room created, Share this key to invite the others: " + key + "\n";
            sendSysteMessage(clientSocket,keyMessage);

        }else if(privateChoice == "2") {
            // Join the private room
            string joinrooom = "Enter the name of private room:\n";
            sendSysteMessage(clientSocket,joinrooom);

            bytesReceived = recv(clientSocket,buffer,sizeof(buffer),0);
            if (bytesReceived <= 0) {
                cerr << "Failed to receive data and client disconnected" << endl;
                close(clientSocket); 
                return;
            }

            roomname = string(buffer,bytesReceived);
            memset(buffer,0,sizeof(buffer));  // clear buffer

            string keyMessage = "Enter the room key:\n";
            sendSysteMessage(clientSocket,keyMessage);

            bytesReceived = recv(clientSocket,buffer,sizeof(buffer),0);
            if (bytesReceived <= 0) {
                cerr << "Failed to receive data and client disconnected" << endl;
                close(clientSocket); 
                return;
            }

            string enteredkey(buffer,bytesReceived);
            memset(buffer,0,sizeof(buffer));  // clear buffer
            
            {
                 lock_guard<mutex> lock(roomMutex);
                if(chatRooms.find(roomname) == chatRooms.end() || 
                   !chatRooms[roomname].isprivate || 
                   chatRooms[roomname].key != enteredkey) {
                    sendSysteMessage(clientSocket, "Invalid room name or key.");
                    close(clientSocket);
                    return;
                }
                chatRooms[roomname].clients.push_back(clientSocket);
                sendSysteMessage(clientSocket, "Successfully joined private room: " + roomname);
            }
        }

    }else {
        sendSysteMessage(clientSocket, "Invalid choice. Please choose 1 or 2.");
        close(clientSocket);
        return;
    }

    {
        lock_guard<mutex> lock(roomMutex);
        auto& room = chatRooms[roomname];
        for(int client: room.clients) {
            if(client != clientSocket) {
                sendSysteMessage(client, "New user joined the room");
            }
        }
    }

    // main loop for recieving and forwarding message
    while(1) {
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cerr << "Failed to receive data and client disconnected" << endl;
            break;
        }
        string message(buffer,bytesReceived);
        cout << "Message from client: " << message << endl;
        sendMessage(roomname,message,clientSocket);
        memset(buffer,0,sizeof(buffer));

    }

        {
        lock_guard<mutex> lock(roomMutex);
        auto& room = chatRooms[roomname];
        room.clients.erase(remove(room.clients.begin(), room.clients.end(), clientSocket), 
                          room.clients.end());
        
        // Notify remaining users about departure
        for(int client: room.clients) {
            sendSysteMessage(client, "A user has left the room");
        }
        }

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



    while(1) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            cerr << "Failed to accept connection." << endl;
            close(serverSocket);
            return -1;
        }else {
            cout << "Client is connected" << endl;
        }

        thread clientThread(interactWithClient, clientSocket);
        clientThread.detach();
    }
    
    close(serverSocket);

    return 0;
}
