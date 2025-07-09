#include "communication.h"

int start_communication(int socket_fd, client_config config, 
                        const std::string& client_addr,
                        const std::string& server_addr) {

    send_iam_message(socket_fd, config, client_addr, server_addr);

    if (config.is_auto_player) {
        auto p = std::make_shared<player>();

        if (mess_handler(socket_fd, p, config.position, 
                            config.is_auto_player, nullptr, nullptr,
                            client_addr, server_addr) < 0) {

            return -1;
        }
    }
    else {
        if (communication_non_auto(socket_fd, config.position,
                                client_addr, server_addr) < 0) {
            return -1;
        }
    }

    return 0;
}