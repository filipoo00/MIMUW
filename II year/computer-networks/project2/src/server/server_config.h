#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

#include <string>

struct server_config {
    uint16_t port = 0;
    std::string game_file;
    int timeout = 5;
};

#endif // SERVER_CONFIG_H
