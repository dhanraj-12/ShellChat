# Chat Room Application

## Description

The **Chat Room Application** is a simple multi-client chatroom built entirely in C++. It allows multiple users to communicate in real-time within a local network or remotely via cloud hosting. This project utilizes C++'s built-in socket libraries for networking and multithreading for handling multiple client connections simultaneously.

## Features

- **Multiple Clients**: Supports multiple users chatting concurrently.
- **Public and Private Room Mechanism**: Users can join public rooms freely or create their own public or private chat rooms.
- **Room Selection and Creation**: Users can select from available public rooms or create a new room. Private rooms require a unique private key for access, which must be shared by the room creator.
- **Message Broadcasting**: Messages from one client are sent to all other connected clients within the same room.
- **Threaded Client Handling**: Each client is managed in its own thread for smooth communication.
- **Client Disconnection Handling**: Removes disconnected clients and continues broadcasting messages for active users.

## Technologies Used

- **C++ Standard Library Sockets** (`<sys/socket.h>`, `<netinet/in.h>`, etc.)
- **Multithreading** with the C++ Standard Library's `thread`

## How it Works

### Server

The server listens for incoming client connections on a specified port (e.g., 8080). When a client connects, the server starts a new thread to handle communication with that client. Messages received from any client are broadcast to all other connected clients.

### Client

Each client connects to the server using a socket. Once connected, clients can send and receive messages. Clients can exit the chat by typing `quit`, which closes their connection to the server.

## Requirements

- A Linux-based system (can be adapted for Windows with minor changes)
- C++ compiler (e.g., `g++`)
- Basic understanding of networking (TCP/IP)

## Installation

### Step 1: Clone the Repository
```bash
git clone https://github.com/your-username/chat-room-application.git
cd chat-room-application
```

### Step 2: Compile the Code

```bash
g++ server.cpp -o server -lpthread
g++ client.cpp -o client
```

### Step 3: Run the Server

```bash
./server
```

### Step 4:  Run the Client
```bash
./client
```
## Known Issues

- Clients may experience slight delays in message broadcasting with a large number of users.
- Server performance can be affected by very high traffic.

## Contributing

Contributions are welcome! Feel free to fork the repository, make changes, and submit pull requests.

## License

This project is licensed under the MIT License. See the [LICENSE](./LICENSE) file for more details.
