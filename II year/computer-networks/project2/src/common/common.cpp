#include <iostream>
#include <cstring>
#include <stdexcept>
#include <system_error>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <memory>
#include <regex>

#include "common.h"
#include "err.h"


ssize_t network_utility::readn(int fd, std::string& buf) {
    ssize_t nread;
    char c;
    size_t totalRead = 0;
    std::string delimiter = "\r\n";

    while (true) {
        nread = read(fd, &c, 1);
        if (nread < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return -3;
            }

            return -1;
        } 
        else if (nread == 0) {
            return -2;
        }
        buf += c;
        totalRead += nread;

        if (buf.size() >= delimiter.size()) {
            std::string tail = buf.substr(buf.size() - delimiter.size());
            if (tail == delimiter) {
                buf.resize(buf.size() - delimiter.size());
                return totalRead - delimiter.size();
            }
        }
    }

    return totalRead;
}

ssize_t network_utility::writen(int fd, std::vector<char>& buf) {
    size_t n = buf.size();
    size_t nleft = n;
    ssize_t nwritten;
    const char* ptr = buf.data();

    while (nleft > 0) {
        nwritten = write(fd, ptr, nleft);
        if (nwritten < 0) {
            if (errno == EPIPE || errno == EINTR || errno == ECONNRESET) {
                return -2;
            }
            return -1;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return n - nleft;
}

int network_utility::char_to_index(char seat) {
    switch (seat) {
        case 'N': return 0;
        case 'E': return 1;
        case 'S': return 2;
        case 'W': return 3;
        default: return -1;
    }
}

char network_utility::index_to_char(int index) {
    switch (index) {
        case 0: return 'N';
        case 1: return 'E';
        case 2: return 'S';
        case 3: return 'W';
        default: throw std::invalid_argument("Invalid player index");
    }
}

std::string network_utility::extract_first_card(const std::string& cards) {
    std::regex card_pattern(R"((10|[2-9JQKA])([CDHS]))"); 
    std::smatch matches;

    if (std::regex_search(cards, matches, card_pattern)) {
        return matches[0];
    }

    return "";
}

int network_utility::check_suit(char suit) {
    if (suit != 'C' && suit != 'D' && suit != 'H' && suit != 'S') {
        return -1;
    }

    return 0;
}

int network_utility::check_position(char position) {
    if (position != 'N' && position != 'E' && position != 'S' && 
                                                position != 'W') {
        return -1;
    }

    return 0;
}


void network_utility::get_socket_address(int socket_fd, bool local,
                                            std::string& address) {
    sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if (local ? getsockname(socket_fd, (struct sockaddr *)&addr, &len) :
                getpeername(socket_fd, (struct sockaddr *)&addr, &len)) {
        error("get socket addrres");
    }

    if (addr.ss_family == AF_INET6) {
        sockaddr_in6 *s = (sockaddr_in6 *)&addr;
        char ip[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &s->sin6_addr, ip, sizeof(ip));
        address = ip;
        address += ":";
        address += std::to_string(ntohs(s->sin6_port));
    }
}

void network_utility::log_message(const std::string& from_addr, 
                                const std::string& to_addr, 
                                const std::string& message) {

    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = 
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%dT%H:%M:%S")
            << '.' << std::setfill('0') << std::setw(3) << milliseconds;

    std::string visible_message = message;
    size_t pos = 0;
    while ((pos = visible_message.find("\r\n", pos)) != std::string::npos) {
        visible_message.replace(pos, 2, "\\r\\n");
        pos += 4;
    }

    std::cout << "[" << from_addr << "," << to_addr << "," << time_ss.str() << "] " << visible_message << std::endl;
}

void network_utility::write_log(std::string& message, bool received_message,
                                const std::string& from_addr, 
                                const std::string& to_addr) {

    std::string local_addr;
    std::string remote_addr;
    if (received_message) {
        message += "\r\n";
        local_addr = to_addr;
        remote_addr = from_addr;
    }
    else {
        local_addr = from_addr;
        remote_addr = to_addr;
    }

    network_utility::log_message(local_addr, remote_addr, message);
}

bool network_utility::safe_stoi(const std::string& str, size_t& out) {
    try {
        unsigned long long value = std::stoull(str);
        out = static_cast<size_t>(value);
        return true;
    } catch (const std::invalid_argument& ia) {}
    catch (const std::out_of_range& orr) {}
    return false;
}

int network_utility::check_correctness(std::vector<std::string> cards_vector,
                                        size_t max_size) {
    if (cards_vector.size() > max_size) {
        return -1;
    }
    for (size_t i = 0; i < cards_vector.size(); i++) {
        std::string card = cards_vector[i];
        char suit = card.back();
        if (network_utility::check_suit(suit) < 0) {
            return -1;
        }

        card.pop_back();
        auto it = card_values.find(card);
        if (it == card_values.end()) {
            return -1;
        }
    }

    return 0;
}

std::vector<std::string> network_utility::split_cards(
                            const std::string& cards_string) {
                                
    std::regex card_pattern(R"((10|[2-9JQKA])([CDHS]))");
    std::smatch matches;

    std::vector<std::string> cards_vector;

    std::string::const_iterator searchStart(cards_string.cbegin());
    while (std::regex_search(searchStart, cards_string.cend(), 
                                matches, card_pattern)) {

        std::string card_value = matches[1];
        char suit = matches[2].str()[0];
        std::string card = card_value + suit;
        
        cards_vector.push_back(card);

        searchStart = matches.suffix().first;
    }

    return cards_vector;
}