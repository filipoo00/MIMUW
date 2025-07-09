#ifndef MESS_HANDLER_H
#define MESS_HANDLER_H

#include <array>
#include <cstring>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../common/common.h"
#include "../common/err.h"
#include "client_config.h"
#include "../common/player.h"

void send_iam_message(int socket_fd, client_config config, 
                        const std::string& client_addr, 
                        const std::string& server_addr);
int receive_deal_message(int socket_fd, std::shared_ptr<player> player,
                        bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr);
int receive_trick_message(int socket_fd, std::shared_ptr<player> player, 
                        size_t index, bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr);
int send_trick_message(int socket_fd, std::shared_ptr<player> player, 
                        size_t index, bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr);
int receive_taken_message(int socket_fd, size_t index, char position, 
                        std::shared_ptr<player>, bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr);
int receive_score_message(int socket_fd, bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr);
int receive_total_message(int socket_fd, bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr);
int receive_taken_or_trick_message(int socket_fd, size_t trick_number, 
                        char my_position, std::shared_ptr<player> player,
                        bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr);

#endif // MESS_HANDLER_H