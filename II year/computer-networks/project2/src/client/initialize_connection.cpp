#include <iostream>
#include <stdexcept>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include <memory>
#include <netdb.h>

#include "../common/common.h"
#include "../common/err.h"

int initialize_connection(const std::string& server_address, 
                            uint16_t port, bool is_IPv4, bool is_IPv6,
                            std::string& client_address_and_port,
                            std::string& server_address_and_port) {

    int socket_fd = -1;
    int family = AF_UNSPEC;
    if (is_IPv4) {
        family = AF_INET;
    } 
    else if (is_IPv6) {
        family = AF_INET6;
    }

    struct addrinfo hints{}, *result = nullptr;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string port_str = std::to_string(port);
    int errcode = getaddrinfo(server_address.c_str(), port_str.c_str(),
                                &hints, &result);
    if (errcode != 0) {
        error("getaddrinfo failed: " + std::string(gai_strerror(errcode)));
    }

    std::unique_ptr<struct addrinfo, void (*)(struct addrinfo*)>
                                res(result, freeaddrinfo);


    for (struct addrinfo* rp = res.get(); rp != nullptr; rp = rp->ai_next) {
        socket_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socket_fd == -1) {
            continue;
        }

        if (connect(socket_fd, rp->ai_addr, rp->ai_addrlen) == 0) {
            char server_addr[INET6_ADDRSTRLEN];
            if (rp->ai_family == AF_INET) {
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;
                inet_ntop(AF_INET, &ipv4->sin_addr, server_addr, 
                                                INET_ADDRSTRLEN);
            } 
            else if (rp->ai_family == AF_INET6) {
                struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)rp->ai_addr;
                inet_ntop(AF_INET6, &ipv6->sin6_addr, server_addr, 
                                                INET6_ADDRSTRLEN);
            }
            server_address_and_port = std::string(server_addr) 
                                            + ":" + std::to_string(port);
            struct sockaddr_storage local_addr{};
            socklen_t addr_size = sizeof(local_addr);
            if (getsockname(socket_fd, (struct sockaddr*)&local_addr, 
                            &addr_size) == 0) {
                char client_addr[INET6_ADDRSTRLEN];
        
                if (local_addr.ss_family == AF_INET) {
                    struct sockaddr_in *addr_in = 
                                        (struct sockaddr_in *)&local_addr;
                    inet_ntop(AF_INET, &(addr_in->sin_addr), client_addr,
                                        sizeof(client_addr));
                    int local_port = ntohs(addr_in->sin_port);
                    client_address_and_port = std::string(client_addr) 
                                        + ":" + std::to_string(local_port);
                } else {
                    struct sockaddr_in6 *addr_in6 = 
                                        (struct sockaddr_in6 *)&local_addr;
                    inet_ntop(AF_INET6, &(addr_in6->sin6_addr), client_addr,
                                            sizeof(client_addr));
                    int local_port = ntohs(addr_in6->sin6_port);
                    client_address_and_port = std::string(client_addr) 
                                        + ":" + std::to_string(local_port);
                }
            }
            return socket_fd;
        }

        close(socket_fd);
        socket_fd = -1;
    }

    error("Failed to connect to any address");

    return -1;
}
