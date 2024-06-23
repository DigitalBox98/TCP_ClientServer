#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

struct ClientInfo 
{
    int socket;
    sockaddr_in address;
};

class Server 
{
public:
    Server(int port) : serverSocket(-1), port(port) {}

    void start() {
        if (!initialize())
            return;

        std::cout << "Server is listening on port " << port << std::endl;

        while (true) {
            sockaddr_in clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
            if (clientSocket < 0) {
                std::cerr << "Failed to accept connection." << std::endl;
                continue;
            }

            std::cout << "New connection from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

            {
                std::lock_guard<std::mutex> lock(clientsMutex);
                clients.push_back({clientSocket, clientAddr});
            }

            std::thread clientThread(&Server::handleClient, this, clientSocket, clientAddr);
            clientThread.detach(); // Detach the thread so it can run independently
        }
    }

private:
    int serverSocket;
    int port;
    std::vector<ClientInfo> clients;
    std::mutex clientsMutex;

    bool initialize() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            std::cerr << "Failed to create server socket." << std::endl;
            return false;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
            std::cerr << "Failed to bind to port " << port << "." << std::endl;
            return false;
        }

        if (listen(serverSocket, 5) < 0) {
            std::cerr << "Failed to listen on server socket." << std::endl;
            return false;
        }

        return true;
    }

    void handleClient(int clientSocket, sockaddr_in clientAddr) {
        char buffer[1024] = {0};
        int valread;

        while ((valread = read(clientSocket, buffer, 1024)) > 0) {
            std::string message(buffer, 0, valread);
            std::cout << "Received from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << " - " << message << std::endl;

            // Redistribute the message to all connected clients
            std::lock_guard<std::mutex> lock(clientsMutex);
            for (const auto& client : clients) {
                send(client.socket, buffer, valread, 0);
            }
            std::fill(std::begin(buffer), std::end(buffer), 0); // Clear buffer
        }

        std::cout << "Client disconnected: " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

        // Remove client from the list
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.erase(std::remove_if(clients.begin(), clients.end(), [clientSocket](const ClientInfo& info) {
                return info.socket == clientSocket;
            }), clients.end());
        }

        close(clientSocket);
    }
};

int main() {
    int port = 8080;
    Server server(port);
    server.start();

    return 0;
}

