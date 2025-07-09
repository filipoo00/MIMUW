#include <iostream>
#include <vector>
#include <netinet/tcp.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <memory>

#include "command_line_parser.h"
#include "client_config.h"
#include "initialize_connection.h"
#include "../common/common.h"
#include "mess_handler.h"
#include "../common/player.h"
#include "non_auto_player.h"
#include "communication.h"


int main(int argc, char* argv[]) {
    command_line_parser parser;
    client_config config;

    config = parser.parse(argc, argv);
    std::string client_addr;
    std::string server_addr;

    int socket_fd = initialize_connection(config.host, config.port, 
                                        config.use_IPv4, config.use_IPv6,
                                        client_addr, server_addr);
    if (socket_fd < 0) {
        error("Failed to connect.");
    }

    if (start_communication(socket_fd, config, client_addr, server_addr) < 0) {
        return 1;
    }

    return 0;
}
