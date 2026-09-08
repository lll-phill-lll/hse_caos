#include <iostream>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 55555

void handle_client(int client_fd, sockaddr_in client_addr) {
    std::cout << "Client connected: " << inet_ntoa(client_addr.sin_addr) 
              << ":" << ntohs(client_addr.sin_port) 
              << " (fd: " << client_fd << ")" << std::endl;
    
    char buf[1024];
    ssize_t n;
    
    while ((n = read(client_fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, n);
        write(client_fd, buf, n);
    }
    
    std::cout << "Client disconnected (fd: " << client_fd << ")" << std::endl;
    close(client_fd);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    
    int res = bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    if (res < 0) {
         std::cerr << "Can't bind: " << std::strerror(errno) << std::endl;
    }
    listen(server_fd, 5);
    
    std::cout << "Server listening on port " << PORT << std::endl;
    
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        // read();
        
        std::thread(handle_client, client_fd, client_addr).detach();
    }
    
    close(server_fd);
    return 0;
}

