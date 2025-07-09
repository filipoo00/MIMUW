#ifndef CLIENT_CONFIG_H
#define CLIENT_CONFIG_H

#include <string>

struct client_config {
    std::string host;
    uint16_t port;
    bool is_port;
    bool use_IPv4;
    bool use_IPv6;
    char position;
    bool is_auto_player;
};

#endif // CLIENT_CONFIG_H
