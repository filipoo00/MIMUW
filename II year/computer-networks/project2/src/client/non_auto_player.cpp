#include "non_auto_player.h"

std::binary_semaphore sem_server_ready(0);
std::string user_command;
std::atomic<bool> is_user_command_allowed(false);


void display_trick_cards(std::shared_ptr<player> p) {
    std::vector<std::vector<std::string>> taken_tricks =
                                        p->get_taken_tricks();
    std::ostringstream oss;
    for (size_t i = 0; i < taken_tricks.size(); i++) {
        size_t size_trick = taken_tricks[i].size();
        for (size_t j = 0; j < size_trick; j++) {
            if (j > 0) {
                oss << ", ";
            }
            oss << taken_tricks[i][j];
        }
        oss << std::endl;
    }

    std::cout << oss.str();
}

void display_user_cards(std::shared_ptr<player> p) {
    std::vector<std::string> cards_vector = p->get_available_cards();
    std::ostringstream oss;
    for (size_t i = 0; i < cards_vector.size(); i++) { 
        if (i > 0) {
            oss << ", ";
        }
        oss << cards_vector[i];
    }
    std::cout << oss.str() << std::endl;
}

int mess_handler(int socket_fd, std::shared_ptr<player> p, char position,
                bool is_auto_player, int* return_code, int* shutdown_pipe, 
                const std::string& client_addr, 
                const std::string& server_addr) {

    bool received_score = false;
    bool received_total = false;
    
    while (true) {
        if (receive_deal_message(socket_fd, p, is_auto_player,
                                client_addr, server_addr) < 0) {
            break;
        }
    
        received_score = false;
        received_total = false;

        size_t i = 0;
        for (; i < 13; i++) {
            if (i == 0) {
                while(true) {
                    if (receive_taken_or_trick_message(socket_fd, i+1, 
                                    position, p, is_auto_player, client_addr,
                                    server_addr) == 1) {
                        
                        break;
                    }
                    i++;
                }
            }
            else {
                receive_trick_message(socket_fd, p, i+1, is_auto_player,
                                    client_addr, server_addr);
            }
            
            if (!is_auto_player) {
                is_user_command_allowed = true;
                sem_server_ready.acquire();
            }

            send_trick_message(socket_fd, p, i+1, is_auto_player,
                                client_addr, server_addr);


            receive_taken_message(socket_fd, i+1, position, p, is_auto_player,
                                    client_addr, server_addr);
        }
        receive_score_message(socket_fd, is_auto_player,
                                client_addr, server_addr);
        received_score = true;

        receive_total_message(socket_fd, is_auto_player,
                                client_addr, server_addr);
        received_total = true;

        if (!is_auto_player) {
            p->erase_taken_tricks();
        }
    }

    close(socket_fd);

    if (!received_score || !received_total) {
        if (!is_auto_player) {
            *return_code = -1;
            const char *msg = "stop";
            write(shutdown_pipe[1], msg, strlen(msg));
        }
        return -1;
    }

    if (!is_auto_player) {
        *return_code = 0;
        const char *msg = "stop";
        write(shutdown_pipe[1], msg, strlen(msg));
    }

    return 0;
}

void user_input_thread(std::shared_ptr<player> p, int* shutdown_pipe) {
    std::string command;
    std::vector<struct pollfd> fds(2);

    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    fds[1].fd = shutdown_pipe[0];
    fds[1].events = POLLIN;

    while (true) {
        int ret = poll(fds.data(), fds.size(), -1);
        if (ret < 0) {
            error("poll failed");
        }

        if (fds[1].revents & POLLIN) {
            break;
        }
        if (fds[0].revents & POLLIN) {
            std::getline(std::cin, command);
            if (command.starts_with("!")) {
                if (is_user_command_allowed) {
                    user_command = command.substr(1);

                    if (p->check_if_correct_card(user_command) == -1) {
                        std::cout << "Not the right suit." << std::endl;
                        continue;
                    }

                    if (p->set_played_card(user_command) == -1) {
                        std::cout << "Not the right card." << std::endl;
                        continue;
                    }

                    is_user_command_allowed = false;
                    sem_server_ready.release();
                }
                else {
                    std::cout << "Not the right time to place a card." << std::endl;
                }
            }
            else if (command == "tricks") {
                display_trick_cards(p);
            }
            else if (command == "cards") {
                display_user_cards(p);
            }
            else {
                std::cout << "Unknown command." << std::endl;
            }
        }
    }
}

int communication_non_auto(int socket_fd, char position, 
                        const std::string& client_addr,
                        const std::string& server_addr) {

    auto p = std::make_shared<player>();
    int return_code = 0;

    int shutdown_pipe[2];
    if (pipe(shutdown_pipe) != 0) {
        error("pipe failed");
    }

    std::thread server(mess_handler, socket_fd, p, position, false, 
                                    &return_code, shutdown_pipe,
                                    client_addr, server_addr);
    std::thread user(user_input_thread, p, shutdown_pipe);


    server.join();
    user.join();

    if (return_code < 0) {
        return -1;
    }

    return 0;
}
