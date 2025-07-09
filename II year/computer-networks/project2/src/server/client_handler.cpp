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
#include <condition_variable>
#include <barrier>
#include <semaphore>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <poll.h>

#include "../common/common.h"
#include "../common/err.h"
#include "client_handler.h"
#include "game_state.h"

std::mutex mutex;
std::vector<std::thread> client_threads;
std::array<int, 4> client_sockets = {-1, -1, -1, -1};

std::vector<game_state> game_states = std::vector<game_state>(4);

int current_turn = 0;

std::vector<int> scores(4);

std::barrier<> deal_start_barrier(4);
std::barrier<> deal_end_barrier(4);
std::barrier<> trick_start_barrier(4);
std::barrier<> trick_end_barrier(4);

std::mutex log_mutex;

void set_socket_timeout(int socket_fd, int timeout_sec) {
    struct timeval timeout;
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, 
                    (char *)&timeout, sizeof(timeout)) < 0) {
        error("setsockopt failed");
    }
}

void send_busy_message(int client_socket, 
                        const std::string& server_addr,
                        const std::string& client_addr) {
    std::string busy_msg = "BUSY";

    for (size_t i = 0; i < game_states.size(); ++i) {
        if (game_states[i].get_seat()) {
            busy_msg += network_utility::index_to_char(i);
        }
    }

    busy_msg += "\r\n";

    std::vector<char> msg_data(busy_msg.begin(), busy_msg.end());
    ssize_t len = network_utility::writen(client_socket, msg_data);
    if (len < 0) {
        error("bad send func, send busy message.");
    }

    std::lock_guard<std::mutex> lock(log_mutex);
    network_utility::write_log(busy_msg, false, server_addr, client_addr);
}

int receive_iam_message(int client_socket, 
                        const std::string& server_addr,
                        const std::string& client_addr) {
    std::string message;
    ssize_t len = network_utility::readn(client_socket, message);
    if (len < 0) {
        error("bad read func, receive iam message.");
    }

    if (message.size() != 4 || message.substr(0, 3) != "IAM") {
        return -1;
    }

    int seat = network_utility::char_to_index(message[3]);
    if (seat < 0 || game_states[seat].get_seat()) {
        send_busy_message(client_socket, server_addr, client_addr);
        return -1;
    }

    game_states[seat].set_seat(true);

    std::lock_guard<std::mutex> lock(log_mutex);
    network_utility::write_log(message, true, server_addr, client_addr);

    return seat;
}

int send_deal_message(int client_socket, int seat, 
                        std::shared_ptr<deal> current_deal, 
                        const std::string& server_addr,
                        const std::string& client_addr) {

    std::string deal_msg = "DEAL";
    deal_msg += std::to_string(current_deal->get_deal_type());
    deal_msg += current_deal->get_lead_player();
    deal_msg += current_deal->get_player(seat).get_cards();
    deal_msg += "\r\n";

    std::vector<char> msg_data(deal_msg.begin(), deal_msg.end());
    ssize_t len = network_utility::writen(client_socket, msg_data);
    if (len < 0) {
        if (len == -2) {
            return -2;
        }
        error("bad write func, send deal message.");
    }

    std::lock_guard<std::mutex> lock(log_mutex);
    network_utility::write_log(deal_msg, false, server_addr, client_addr);

    return 0;
}

int send_wrong_message(int client_socket, size_t trick_number, 
                        const std::string& server_addr,
                        const std::string& client_addr) {

    std::string wrong_msg = "WRONG";
    wrong_msg += std::to_string(trick_number);
    wrong_msg += "\r\n";

    std::vector<char> msg_data(wrong_msg.begin(), wrong_msg.end());
    ssize_t len = network_utility::writen(client_socket, msg_data);
    if (len < 0) {
        error("bad write func, send wrong message.");
    }

    std::lock_guard<std::mutex> lock(log_mutex);
    network_utility::write_log(wrong_msg, false, server_addr, client_addr);

    return 0;
}

int send_trick_message(int client_socket, std::shared_ptr<trick> current_trick,
                        size_t index, 
                        const std::string& server_addr,
                        const std::string& client_addr) {

    std::string trick_msg = "TRICK";
    trick_msg += std::to_string(index);
    trick_msg += current_trick->get_cards_string();
    trick_msg += "\r\n";

    std::vector<char> msg_data(trick_msg.begin(), trick_msg.end());
    ssize_t len = network_utility::writen(client_socket, msg_data);
    if (len < 0) {
        if (len == -2) {
            return -2;
        }
        error("bad write func, send trick message.");
    }

    std::lock_guard<std::mutex> lock(log_mutex);
    network_utility::write_log(trick_msg, false, server_addr, client_addr);
    return 0;
}

int receive_trick_message(int client_socket, 
                        std::shared_ptr<trick> current_trick,
                        size_t index,
                        const std::string& server_addr,
                        const std::string& client_addr) {
    
    while (1) {
        std::string message;
        ssize_t len = network_utility::readn(client_socket, message);
        if (len < 0) {
            if (len == -3) {
                return -3;
            }
            if (len == -2) {
                return -2;
            }
            error("bad write func, receive trick message.");
        }


        if (message.size() < 5 && message.substr(0, 5) != "TRICK") {
            network_utility::write_log(message, true, server_addr, 
                                        client_addr);
            close(client_socket);
            return -1;
        }
        size_t number_length = (index < 10) ? 1 : 2;
        size_t start_index_of_cards = 5 + number_length;
        if (message.size() < start_index_of_cards) {
            network_utility::write_log(message, true, server_addr, 
                                        client_addr);
            close(client_socket);
            return -1;
        }

        std::string cards = message.substr(start_index_of_cards);
        std::vector<std::string> cards_vector =
                                network_utility::split_cards(cards);
        if (network_utility::check_correctness(cards_vector, 1) < 0) {
            send_wrong_message(client_socket, index, server_addr, client_addr);
            continue;
        }

        size_t trick_number;
        if (!network_utility::safe_stoi(message.substr(5, number_length), 
                                        trick_number)) {
            send_wrong_message(client_socket, index, server_addr, client_addr);
            continue;
        }

        if (trick_number != index) {
            send_wrong_message(client_socket, index, server_addr, client_addr);
            continue;
        }

        current_trick->add_card_to_cards(cards);

        std::lock_guard<std::mutex> lock(log_mutex);
        network_utility::write_log(message, true, server_addr, client_addr);

        break;
    }

    return 0;
}

int send_taken_message(int client_socket, std::shared_ptr<trick> current_trick,
                        size_t index,
                        const std::string& server_addr,
                        const std::string& client_addr) {

    std::string taken_msg = "TAKEN";
    taken_msg += std::to_string(index);
    taken_msg += current_trick->get_cards_string();
    int winner = current_trick->get_winning_player();
    taken_msg += network_utility::index_to_char(winner);
    taken_msg += "\r\n";

    std::vector<char> msg_data(taken_msg.begin(), taken_msg.end());
    ssize_t len = network_utility::writen(client_socket, msg_data);
    if (len < 0) {
        if (len == -2) {
            return -2;
        }
        error("bad write func, send taken message.");
    }

    std::lock_guard<std::mutex> lock(log_mutex);
    network_utility::write_log(taken_msg, false, server_addr, client_addr);


    return 0;
}

int send_score_message(int client_socket, std::shared_ptr<deal> current_deal,
                        const std::string& server_addr,
                        const std::string& client_addr) {

    std::string score_msg = "SCORE";
    std::vector<int> scores_deal = current_deal->get_scores();
    for (size_t i = 0; i < scores_deal.size(); i++) {
        score_msg += network_utility::index_to_char(i);
        score_msg += std::to_string(scores_deal[i]);
    }
    score_msg += "\r\n";

    std::vector<char> msg_data(score_msg.begin(), score_msg.end());
    ssize_t len = network_utility::writen(client_socket, msg_data);
    if (len < 0) {
        if (len == -2) {
            return -2;
        }
        error("bad write func, send score message.");
    }

    std::lock_guard<std::mutex> lock(log_mutex);
    network_utility::write_log(score_msg, false, server_addr, client_addr);

    return 0;
}

int send_total_message(int client_socket,
                        const std::string& server_addr,
                        const std::string& client_addr) {
    std::string total_msg = "TOTAL";
    for (size_t i = 0; i < scores.size(); i++) {
        total_msg += network_utility::index_to_char(i);
        total_msg += std::to_string(scores[i]);
    }
    total_msg += "\r\n";

    std::vector<char> msg_data(total_msg.begin(), total_msg.end());
    ssize_t len = network_utility::writen(client_socket, msg_data);
    if (len < 0) {
        if (len == -2) {
            return -2;
        }
        error("bad write func, send total message.");
    }

    std::lock_guard<std::mutex> lock(log_mutex);
    network_utility::write_log(total_msg, false, server_addr, client_addr);

    return 0;
}

void wait_for_turn(int player_id,
                std::array<std::counting_semaphore<1>, 4>& player_semaphores) {

    player_semaphores[player_id].acquire();
}

void pass_turn(int player_id, 
                std::array<std::counting_semaphore<1>, 4>& player_semaphores) {
    current_turn = (player_id + 1) % 4;
    player_semaphores[current_turn].release();
}

int deal_handler(int client_socket, int client_position, 
                std::shared_ptr<deal> current_deal,
                std::array<std::counting_semaphore<1>, 4>& player_semaphores,
                const std::string& server_addr,
                const std::string& client_addr) {

    int deal_ret = send_deal_message(client_socket, client_position, 
                                    current_deal, server_addr, client_addr);
    if (deal_ret < 0) {
        if (deal_ret == -2) {
            return -2;
        }
        error("bad write func");
    }

    current_turn = network_utility::char_to_index(current_deal->get_lead_player());

    if (client_position == current_turn) {
        player_semaphores[client_position].release();
    }

    return 0;
}

int trick_handler(int client_socket, int client_position, 
                std::shared_ptr<trick> current_trick, 
                std::shared_ptr<deal> current_deal,
                std::array<std::counting_semaphore<1>, 4>& player_semaphores, 
                size_t index,
                const std::string& server_addr,
                const std::string& client_addr) {

    wait_for_turn(client_position, player_semaphores);

    int ret_trick_recv;

    while (true) {
        int ret_trick_send = send_trick_message(client_socket, 
                                                current_trick,
                                                index + 1, 
                                                server_addr, 
                                                client_addr);

        if (ret_trick_send < 0) {
                
            if (ret_trick_send == -2) {
                return -2;
            }
            error("bad write func");
        }

        ret_trick_recv = receive_trick_message(client_socket,
                                               current_trick, 
                                                index + 1,
                                                server_addr,
                                                client_addr);

        if (ret_trick_recv < 0) {
            if (ret_trick_recv == -3) {
                continue;
            }
            if (ret_trick_recv == -2) {
                return -2;
            }
        }
        break;
    }

    if (current_turn == (network_utility::char_to_index(
                            current_deal->get_lead_player()) + 3) % 4) {
        std::string cards_trick = current_trick->get_cards_string();
        
        std::vector<card> cards = current_trick->get_cards();

        size_t index = 0;
        int highest_value = cards[index].value_int;

        for (size_t i = 1; i < cards.size(); i++) {
            card c = cards[i];
            if (c.suit == cards[0].suit && c.value_int > highest_value) {
                highest_value = c.value_int;
                index = i;
            }
        }
        current_trick->set_winning_player((network_utility::char_to_index(
                                current_deal->get_lead_player()) + index) % 4);

    }

    pass_turn(current_turn, player_semaphores);

    trick_end_barrier.arrive_and_wait();

    return 0;
}

int score_total_handler(int client_socket, std::shared_ptr<deal> current_deal,
                        const std::string& server_addr,
                        const std::string& client_addr) {
    int score_ret = send_score_message(client_socket, current_deal, 
                                        server_addr, client_addr);
    if (score_ret == -2) {
        return -2;
    }

    int total_ret = send_total_message(client_socket, server_addr, client_addr);
    if (total_ret == -2) {
        return -2;
    }
    
    return 0;
}

int send_deal_and_taken_msg(int client_socket, int seat, 
                        std::shared_ptr<deal> current_deal,
                        const std::string& server_addr,
                        const std::string& client_addr) {

    send_deal_message(client_socket, seat, current_deal, 
                        server_addr, client_addr);

    for (size_t i = 0; i < game_states[seat].get_current_trick(); i++) {
        std::shared_ptr<trick> current_trick = 
                                current_deal->get_current_trick(i);
        send_taken_message(client_socket, current_trick, i+1, 
                            server_addr, client_addr);
    }
    return 0;
}

void client_handler(int client_socket, int client_position, game& game,
                std::array<std::counting_semaphore<1>, 4>& player_semaphores,
                int *shutdown_pipe,
                const std::string& server_addr,
                const std::string& client_addr) {

    size_t j = game_states[client_position].get_current_deal();
    for(; j < game.get_how_many_deals(); j++) {

        std::shared_ptr<deal> current_deal = game.get_current_deal(j);

        if (game_states[client_position].get_disconnect_on_score_total_handler()) {
            send_deal_and_taken_msg(client_socket, client_position, current_deal, 
                                                        server_addr, client_addr);
            game_states[client_position].set_disconnect_on_score_total_handler(false);
        }

        if (!game_states[client_position].get_disconnect_on_send_taken_msg()) {
            if (!game_states[client_position].get_disconnect_on_deal_handler() &&
                !game_states[client_position].get_disconnect_on_trick_handler()) {

                deal_start_barrier.arrive_and_wait();
            }

            if (!game_states[client_position].get_disconnect_on_trick_handler()) {

                int deal_hand = deal_handler(client_socket, client_position,
                                            current_deal, player_semaphores, 
                                            server_addr, client_addr);
                if (deal_hand == -2) {
                    game_states[client_position].set_deal(j);
                    game_states[client_position].set_disconnect_on_deal_handler(true);
                    game_states[client_position].set_seat(false);
                    client_sockets[client_position] = -1;
                    return;
                }

                game_states[client_position].set_disconnect_on_deal_handler(false);
            }
        }


        size_t i = game_states[client_position].get_current_trick();
        for (; i < 13; i++) {
            std::shared_ptr<trick> current_trick = 
                                    current_deal->get_current_trick(i);

            if (!game_states[client_position].get_disconnect_on_trick_handler()) {
                trick_start_barrier.arrive_and_wait();
            }
            else {
                send_deal_and_taken_msg(client_socket, client_position, 
                                    current_deal, server_addr, client_addr);
                game_states[client_position].set_disconnect_on_trick_handler(false);
            }

            if (game_states[client_position].get_disconnect_on_send_taken_msg()) {
                send_deal_and_taken_msg(client_socket, client_position, 
                                    current_deal, server_addr, client_addr);
                game_states[client_position].set_disconnect_on_send_taken_msg(false);
            }

            int trick_hand = trick_handler(client_socket, client_position,
                                current_trick, current_deal, player_semaphores, 
                                i, server_addr, client_addr);
            if (trick_hand == -2) {
                game_states[client_position].set_deal(j);
                game_states[client_position].set_trick(i);
                game_states[client_position].set_disconnect_on_trick_handler(true);
                game_states[client_position].set_seat(false);
                player_semaphores[client_position].release();
                client_sockets[client_position] = -1;

                return;
            }

            int ret_taken =  send_taken_message(client_socket, current_trick,
                                                i+1, server_addr, client_addr);
            if (ret_taken == -2) {
                if (i != 12) {
                    game_states[client_position].set_disconnect_on_send_taken_msg(true);
                    game_states[client_position].set_seat(false);
                    game_states[client_position].set_trick(i+1);
                    game_states[client_position].set_deal(j);
                    player_semaphores[client_position].release();
                    client_sockets[client_position] = -1;

                    return;
                }
            }
        }
        if (client_position == (network_utility::char_to_index(
                                current_deal->get_lead_player()) + 3) % 4) { 
            current_deal->calculate_points();
            std::vector<int> scores_deal = current_deal->get_scores();
            for (size_t i = 0; i < scores.size(); i++) {
                scores[i] += scores_deal[i];
            }
        }

        if (client_position == current_turn) {
            player_semaphores[current_turn].acquire();
        }

        game_states[client_position].reset_trick();

        deal_end_barrier.arrive_and_wait();

        int ret_score_total = score_total_handler(client_socket, current_deal, 
                                                    server_addr, client_addr);
        if (ret_score_total == -2) {
            if (j != game.get_how_many_deals() - 1) {
                game_states[client_position].set_disconnect_on_score_total_handler(true);
                game_states[client_position].set_seat(false);
                game_states[client_position].set_deal(j + 1);
                client_sockets[client_position] = -1;

                return;
            }
        }
    }

    deal_end_barrier.arrive_and_wait();


    if (client_position == current_turn) {
        const char *msg = "stop";
        write(shutdown_pipe[1], msg, strlen(msg));
    }
}

void accept_clients(int server_socket, game& game, int timeout, 
                    int *shutdown_pipe) {

    std::array<std::counting_semaphore<1>, 4> player_semaphores = {
        std::counting_semaphore<1>(0),
        std::counting_semaphore<1>(0),
        std::counting_semaphore<1>(0),
        std::counting_semaphore<1>(0)
    };

    std::vector<struct pollfd> fds(2);
    fds[0].fd = server_socket;
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

            struct sockaddr_in6 client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(server_socket, 
                            (struct sockaddr*)&client_addr, &client_len);

            if (client_socket < 0) {
                continue;
            }
            std::string server_addr;
            std::string client_addr_and_port;
            network_utility::get_socket_address(client_socket, true,
                                                server_addr);
            network_utility::get_socket_address(client_socket, false, 
                                                client_addr_and_port);

            set_socket_timeout(client_socket, timeout);

            std::unique_lock<std::mutex> lock(mutex);
            int seat = receive_iam_message(client_socket, server_addr, 
                                                client_addr_and_port);
            if (seat < 0) {
                lock.unlock();
                close(client_socket);
                continue;
            }


            auto pos = std::find(client_sockets.begin(), 
                                    client_sockets.end(), -1);
            if (pos != client_sockets.end()) {
                *pos = client_socket;
                lock.unlock();
                client_threads.emplace_back(std::thread(client_handler, 
                                client_socket, seat, std::ref(game), 
                                std::ref(player_semaphores), shutdown_pipe,
                                server_addr, client_addr_and_port));
            }
        }
    }

    for (auto& th : client_threads) {
        if (th.joinable()) {
            th.join();
        }
    }
}
