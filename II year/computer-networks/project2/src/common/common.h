#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <sys/types.h>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "card_values.h"

class network_utility {
public:
    static ssize_t readn(int fd, std::string& buf);

    static ssize_t writen(int fd, std::vector<char>& buf);

    static struct sockaddr_in get_server_address_ipv4(const std::string& host,
                                                    uint16_t port);

    static struct sockaddr_in6 get_server_address_ipv6(const std::string& host, 
                                                    uint16_t port);

    static int char_to_index(char seat);

    static char index_to_char(int index);

    static std::string extract_first_card(const std::string& cards);

    static int check_suit(char suit);

    static int check_position(char position);

    static void get_socket_address(int socket_fd, bool local, 
                                    std::string& addr);

    static void log_message(const std::string& from_addr, 
                            const std::string& to_addr, 
                            const std::string& message);

    static void write_log(std::string& message, bool received_message,
                            const std::string& from_addr, 
                            const std::string& to_addr);

    static bool safe_stoi(const std::string& str, size_t& out);
    static std::vector<std::string> split_cards(
                                const std::string& cards_string);

    static int check_correctness(std::vector<std::string> cards_vector,
                                     size_t max_size);
};

#endif // COMMON_H
