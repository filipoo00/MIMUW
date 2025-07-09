#ifndef NON_AUTO_PLAYER_H
#define NON_AUTO_PLAYER_H


#include <poll.h>
#include <unistd.h>

#include "mess_handler.h"
#include "../common/player.h"
#include "../common/common.h"



int communication_non_auto(int socket_fd, char position, 
                        const std::string& client_addr,
                        const std::string& server_addr);

int mess_handler(int socket_fd, std::shared_ptr<player> p, char position,
                bool is_auto_player, int* return_code, int* shutdown_pipe, 
                const std::string& client_addr,
                const std::string& server_addr);

#endif // NON_AUTO_PLAYER_H
