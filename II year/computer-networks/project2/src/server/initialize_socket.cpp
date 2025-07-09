#include "initialize_socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

#include "../common/err.h"

constexpr int QUEUE_LENGTH = 5;


int init_socket(uint16_t port) {
    int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        error("cannot create a socket");
    }

    int no = 0;
    if (setsockopt(socket_fd, IPPROTO_IPV6, IPV6_V6ONLY, 
                            &no, sizeof(no)) < 0) {
        close(socket_fd);
        error("setting IPV6_V6ONLY");
    }

    struct sockaddr_in6 server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin6_family = AF_INET6;
    server_address.sin6_addr = in6addr_any;
    server_address.sin6_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *)&server_address, 
                            sizeof(server_address)) < 0) {
        close(socket_fd);
        error("bind");
    }

    if (listen(socket_fd, QUEUE_LENGTH) < 0) {
        close(socket_fd);
        error("listen");
    }

    return socket_fd;
}
