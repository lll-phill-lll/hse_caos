#include <iostream>
#include <thread>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 33333

std::atomic<uint64_t> total_requests{0};

void handle_client(int client_fd) {
    char buf[4096];

    ssize_t n = read(client_fd, buf, sizeof(buf));
    if (n <= 0) {
        close(client_fd);
        return;
    }

    ++total_requests;

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 2\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: close\r\n"
        "\r\n"
        "OK";

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    write(client_fd, response.data(), response.size());

    close(client_fd);
}


void stats_printer() {
    while (true) {
        sleep(5);
        std::cout << "[STATS] Total requests: " << total_requests.load() << std::endl;
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    
    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 128);
    
    std::cout << "Echo server (threads) on port " << PORT << std::endl;
    
    std::thread(stats_printer).detach();
    
    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) continue;
        
        std::thread(handle_client, client_fd).detach();
    }
    
    close(server_fd);

    std::cerr << "Total:" << total_requests.load() << std::endl;
    return 0;
}

