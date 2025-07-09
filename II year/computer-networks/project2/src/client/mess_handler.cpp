#include "mess_handler.h"


void send_iam_message(int socket_fd, client_config config, 
                        const std::string& client_addr, 
                        const std::string& server_addr) {
    std::string iam_msg = "IAM";
    iam_msg += config.position;
    iam_msg += "\r\n";

    std::vector<char> msg_data(iam_msg.begin(), iam_msg.end());
    ssize_t len = network_utility::writen(socket_fd, msg_data);
    if (len < 0) {
        error("bad write func, send iam message");
    }
    if (config.is_auto_player) {
        network_utility::write_log(iam_msg, false, 
                                    client_addr, server_addr);
    }
}

int receive_deal_message(int socket_fd, std::shared_ptr<player> player, 
                        bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr) {

    while (1) {
        std::string message;
        ssize_t len = network_utility::readn(socket_fd, message);
        if (len < 0) {
            if (len == -2) {
                return -1;
            }
            error("bad read func, read deal message");
        }

        if (message.size() < 4 || (message.substr(0, 4) != "DEAL" && 
                message.substr(0, 4) != "BUSY")) {
            
            if (is_auto_player) {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }

        if (message.substr(0, 4) == "BUSY") {

            std::string taken_places = message.substr(4);
            if (taken_places.size() > 4 || taken_places.size() == 0) {
                if (is_auto_player) {
                    network_utility::write_log(message, true, 
                                                client_addr, server_addr);
                }
                continue;
            }
            else {
                bool end = false;
                for (size_t i = 0; i < taken_places.size(); i++) {
                    if (network_utility::check_position(taken_places[i]) < 0) {

                        end = true;
                        break;
                    }
                }
                if (end) {
                    if (is_auto_player) {
                        network_utility::write_log(message, true, 
                                                    client_addr, server_addr);
                    }
                    continue;
                }
            }

            if (!is_auto_player) {
                std::ostringstream oss;
                oss << "Place busy, list of busy places received: ";
                for (size_t i = 0; i < taken_places.size(); i++) {
                    if (i > 0) {
                        oss << ", ";
                    }
                    oss << taken_places[i];
                }
                oss << ".";
                std::cout << oss.str() << std::endl;

            }
            else {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            return -1;
        }

        if (message.substr(0, 4) == "DEAL") {
            if (message.size() < 6) {
                if (is_auto_player) {
                    network_utility::write_log(message, true, 
                                                client_addr, server_addr);
                }
                continue;
            }
            std::string deal_type_string;
            deal_type_string += message[4];
            size_t deal_type;
            if (!network_utility::safe_stoi(deal_type_string, deal_type)) {
                if (is_auto_player) {
                    network_utility::write_log(message, true, 
                                                client_addr, server_addr);
                }
                continue;
            }
            char pos = message[5];
            if (deal_type < 1 || deal_type > 7 || 
                network_utility::check_position(pos) < 0) {
                if (is_auto_player) {
                    network_utility::write_log(message, true, 
                                                client_addr, server_addr);
                }
                continue;
            }
            std::string cards = message.substr(6);
            if (player->add_cards(cards) < 0) {
                if (is_auto_player) {
                    network_utility::write_log(message, true,
                                                client_addr, server_addr);
                }
                continue;
            }

            player->set_cards(cards); 

            if (!is_auto_player) {
                std::vector<std::string> cards_vector = network_utility::split_cards(cards);
                std::ostringstream oss;
                oss << "New deal " << message[4] << ": staring place ";
                oss << message[5] << ", your cards: ";
                for (size_t i = 0; i < cards_vector.size(); i++) { 
                    if (i > 0) {
                        oss << ", ";
                    }
                    oss << cards_vector[i];
                }
                oss << ".";

                std::cout << oss.str() << std::endl;
            }
            else {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            break;
        }

    }

    return 0;
}

int receive_trick_message(int socket_fd, std::shared_ptr<player> player, 
                        size_t trick_number, bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr) {

    while (1) {
        std::string message;
        ssize_t len = network_utility::readn(socket_fd, message);
        if (len < 0) {
            error("bad read func, read trick message.");
        }

        if (message.size() < 5 || message.substr(0, 5) != "TRICK") {
            if (is_auto_player) {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }


        size_t number_length = (trick_number < 10) ? 1 : 2;
        size_t start_index_of_cards = 5 + number_length;
        if (message.size() < start_index_of_cards) {
            if (is_auto_player) {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }
        size_t trick_from_msg;
        if (!network_utility::safe_stoi(message.substr(5, number_length), 
                                        trick_from_msg)) {
            if (is_auto_player) {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }
        if (trick_from_msg != trick_number) {
            if (is_auto_player) {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }

        std::string cards = message.substr(start_index_of_cards);
    
        std::vector<std::string> cards_vector = 
                                network_utility::split_cards(cards);
        if (network_utility::check_correctness(cards_vector, 3) < 0) {
            if (is_auto_player) {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }

        player->set_cards_from_trick(cards);
        player->set_current_trick(cards_vector);
        
    
        if (!is_auto_player) {
            std::ostringstream oss;
            oss << "Trick: (" << trick_number << ") ";
            for (size_t i = 0; i < cards_vector.size(); i++) { 
                if (i > 0) {
                    oss << ", ";
                }
                oss << cards_vector[i];
            }
            oss << std::endl;
            oss << "Available: ";

            cards_vector = player->get_available_cards();
            for (size_t i = 0; i < cards_vector.size(); i++) { 
                if (i > 0) {
                    oss << ", ";
                }
                oss << cards_vector[i];
            }
            
            std::cout << oss.str() << std::endl;   

        }
        else {
            network_utility::write_log(message, true, 
                                        client_addr, server_addr);
        }

        break;
    }

    return 0;
}


int send_trick_message(int socket_fd,  std::shared_ptr<player> player,
                        size_t trick_number, bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr) {

    std::string trick_msg = "TRICK";
    trick_msg += std::to_string(trick_number);
    if (!is_auto_player) {
        trick_msg += player->get_played_card();
    }
    else {
        trick_msg += player->select_card();
    }
    trick_msg += "\r\n";

    std::vector<char> msg_data(trick_msg.begin(), trick_msg.end());
    ssize_t len = network_utility::writen(socket_fd, msg_data);
    if (len < 0) {
        error("bad write func, send trick message");
    }

    if (is_auto_player) {
        network_utility::write_log(trick_msg, false, 
                                    client_addr, server_addr);
    }

    return 0;
}

int receive_taken_message(int socket_fd, size_t trick_number, char my_position,
                        std::shared_ptr<player> player, bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr) {

    while (1) {
        std::string message;
        ssize_t len = network_utility::readn(socket_fd, message);
        if (len < 0) {
            error("bad read func, receive taken message");
        }

        if (message.size() < 5 || message.substr(0, 5) != "TAKEN") {
            if (is_auto_player) {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }
        
        char position = message.back();
        if (network_utility::check_position(position) < 0) {
            if (is_auto_player) {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }
        message.pop_back();
        
        size_t number_length = (trick_number < 10) ? 1 : 2;
        size_t start_index_of_cards = 5 + number_length;

        if (message.size() < start_index_of_cards) {
            if (is_auto_player) {
                message += position;
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }
        size_t trick_from_msg;
        if (!network_utility::safe_stoi(message.substr(5, number_length), 
                                        trick_from_msg)) {
            if (is_auto_player) {
                message += position;
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }
        if (trick_from_msg != trick_number) {
            if (is_auto_player) {
                message += position;
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }

        std::string cards = message.substr(start_index_of_cards);
    
        std::vector<std::string> cards_vector = 
                                network_utility::split_cards(cards);
        if (cards_vector.size() != 4 || 
                network_utility::check_correctness(cards_vector, 4) < 0) {

            if (is_auto_player) {
                message += position;
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }


        if (!is_auto_player) {
            std::ostringstream oss;
            std::vector<std::string> cards_vector = network_utility::split_cards(cards);
            if (position == my_position) {
                player->add_taken_trick(cards_vector);
            }
            oss << "A trick " << trick_number << " is taken by ";
            oss << position << ", cards ";
            for (size_t i = 0; i < cards_vector.size(); i++) { 
                if (i > 0) {
                    oss << ", ";
                }
                oss << cards_vector[i];
            }
            oss << ".";
            std::cout << oss.str() << std::endl;
        }
        else {
            message += position;
            network_utility::write_log(message, true, 
                                        client_addr, server_addr);
        }
        break;
    }

    return 0;
}

int receive_score_message(int socket_fd, bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr) {

    while (1) {
        std::string message;
        ssize_t len = network_utility::readn(socket_fd, message);
        if (len < 0) {
            error("bad read func, receive score message");
        }

        if (message.size() < 5 || message.substr(0, 5) != "SCORE") {
            if (is_auto_player) {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }

        std::map<char, size_t> scores;
        size_t index = 5;
        while (index < message.length()) {
            char position = message[index++];
            if (network_utility::check_position(position) < 0) {
                if (is_auto_player) {
                    network_utility::write_log(message, true, 
                                                client_addr, server_addr);
                }
                continue;
            }
            auto it = scores.find(position);
            if (it != scores.end()) {
                if (is_auto_player) {
                    network_utility::write_log(message, true, 
                                                client_addr, server_addr);
                }
                continue;
            }

            std::string score_str;
            while (index < message.length() && std::isdigit(message[index])) {
                score_str += message[index++];
            }
            size_t score;
            if (!network_utility::safe_stoi(score_str, score)) {
                if (is_auto_player) {
                    network_utility::write_log(message, true, 
                                                client_addr, server_addr);
                }
                continue;
            }
            scores[position] = score;
        }

        if (!is_auto_player) {
            std::ostringstream oss;
            oss << "The scores are:" << std::endl;
            for (auto it : scores) {
                oss << it.first << " | " << it.second;
                oss << std::endl;
            }
            std::cout << oss.str();
        }
        else {
            network_utility::write_log(message, true, 
                                        client_addr, server_addr);
        }
        break;
    }

    return 0;
}

int receive_total_message(int socket_fd, bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr) {
    while (1) {
        std::string message;
        ssize_t len = network_utility::readn(socket_fd, message);
        if (len < 0) {
            error("bad read func, receive total message.");
        }
        
        if (message.size() < 5 || message.substr(0, 5) != "TOTAL") {
            if (is_auto_player) {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }
        
        std::map<char, size_t> scores;
        size_t index = 5;
        while (index < message.length()) {
            char position = message[index++];
            if (network_utility::check_position(position) < 0) {
                if (is_auto_player) {
                    network_utility::write_log(message, true, 
                                                client_addr, server_addr);
                }
                continue;
            }
            auto it = scores.find(position);
            if (it != scores.end()) {
                if (is_auto_player) {
                    network_utility::write_log(message, true, 
                                                client_addr, server_addr);
                }
                continue;
            }

            std::string score_str;

            while (index < message.length() && std::isdigit(message[index])) {
                score_str += message[index++];
            }

            size_t score;
            if (!network_utility::safe_stoi(score_str, score)) {
                if (is_auto_player) {
                    network_utility::write_log(message, true, 
                                                client_addr, server_addr);
                }
                continue;
            }
            scores[position] = score;
        }

        if (!is_auto_player) {
            std::ostringstream oss;
            oss << "The total scores are:" << std::endl;
            for (auto it : scores) {
                oss << it.first << " | " << it.second;
                oss << std::endl;
            }
            std::cout << oss.str();
        }
        else {
            network_utility::write_log(message, true, 
                                        client_addr, server_addr);
        }
        break;
    }

    return 0;
}


int receive_taken_or_trick_message(int socket_fd, size_t trick_number, 
                        char my_position, std::shared_ptr<player> player,
                        bool is_auto_player, 
                        const std::string& client_addr, 
                        const std::string& server_addr) {

    while (1) {
        std::string message;
        ssize_t len = network_utility::readn(socket_fd, message);
        if (len < 0) {
            error("bad read func, receive taken message");
        }

        if (message.size() < 5 || (message.substr(0, 5) != "TAKEN" && 
                                    message.substr(0, 5) != "TRICK")) {
            if (is_auto_player) {
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }
        bool is_taken = false;
        char position = my_position;
        if (message.substr(0, 5) == "TAKEN") {
            is_taken = true;
            position = message.back();
            if (network_utility::check_position(position) < 0) {
                if (is_auto_player) {
                    network_utility::write_log(message, true, 
                                                client_addr, server_addr);
                }
                continue;
            }
            message.pop_back();
        }
        
        size_t number_length = (trick_number < 10) ? 1 : 2;
        size_t start_index_of_cards = 5 + number_length;
        if (message.size() < start_index_of_cards) {
            if (is_auto_player) {
                if (is_taken) {
                    message += position;
                }
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }
        size_t trick_from_msg;
        if (!network_utility::safe_stoi(message.substr(5, number_length), 
                                        trick_from_msg)) {
            if (is_auto_player) {
                if (is_taken) {
                    message += position;
                }
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }
        if (trick_from_msg != trick_number) {
            if (is_auto_player) {
                if (is_taken) {
                    message += position;
                }
                network_utility::write_log(message, true, 
                                            client_addr, server_addr);
            }
            continue;
        }

        std::string cards;
        std::vector<std::string> cards_vector;

        if (message.size() > start_index_of_cards) {
            cards = message.substr(start_index_of_cards);
        
            cards_vector = network_utility::split_cards(cards);
            if (is_taken) {
                if (cards_vector.size() != 4 || 
                        network_utility::check_correctness(cards_vector, 4) < 0) {
        
                    if (is_auto_player) {
                        message += position;
                        network_utility::write_log(message, true, 
                                                    client_addr, server_addr);
                    }
                    continue;
                }
                int pos = network_utility::char_to_index(my_position);
                if (is_auto_player) {
                    player->erase_from_cards_by_suit(cards_vector[pos]);
                }
                else {
                    player->erase_from_available_cards(cards_vector[pos]);
                }
            }
            else {
                if (network_utility::check_correctness(cards_vector, 3) < 0) {
                    if (is_auto_player) {
                        network_utility::write_log(message, true, 
                                                    client_addr, server_addr);
                    }
                    continue;
                }

                player->set_cards_from_trick(cards);
                player->set_current_trick(cards_vector);
            }
        }
        if (!is_auto_player) {
            if (is_taken) {
                std::ostringstream oss;
                if (position == my_position) {
                    player->add_taken_trick(cards_vector);
                }
                oss << "A trick " << trick_number << " is taken by ";
                oss << position << ", cards ";
                for (size_t i = 0; i < cards_vector.size(); i++) { 
                    if (i > 0) {
                        oss << ", ";
                    }
                    oss << cards_vector[i];
                }
                oss << ".";
                std::cout << oss.str() << std::endl;
            }
            else {
                std::ostringstream oss;
                oss << "Trick: (" << trick_number << ") ";
                for (size_t i = 0; i < cards_vector.size(); i++) { 
                    if (i > 0) {
                        oss << ", ";
                    }
                    oss << cards_vector[i];
                }
                oss << std::endl;
                oss << "Available: ";

                cards_vector = player->get_available_cards();
                for (size_t i = 0; i < cards_vector.size(); i++) { 
                    if (i > 0) {
                        oss << ", ";
                    }
                    oss << cards_vector[i];
                }
                
                std::cout << oss.str() << std::endl;   
            }
        }
        else {
            if (is_taken) {
                message += position;
            }
            network_utility::write_log(message, true, 
                                        client_addr, server_addr);
        }
        if (!is_taken) {
            return 1;
        }
        break;
    }

    return 0;
}