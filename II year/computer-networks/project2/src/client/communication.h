#ifndef COMMUNICATION_H
#define COMMUNICATION_H


#include "mess_handler.h"
#include "../common/player.h"
#include "../common/common.h"
#include "non_auto_player.h"

int start_communication(int socket_fd, client_config config, 
                        const std::string& client_addr,
                        const std::string& server_addr);

#endif // COMMUNICATION_H
