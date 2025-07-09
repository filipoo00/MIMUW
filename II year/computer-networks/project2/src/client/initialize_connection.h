#ifndef INITIALIZE_CONNECTION_H
#define INITIALIZE_CONNECTION_H

#include <string>

int initialize_connection(const std::string& server_address, uint16_t port,
                            bool is_IPv4, bool is_IPv6, 
                            std::string& client_addrres_and_port,
                            std::string& server_address_and_port);

#endif // INITIALIZE_CONNECTION_H
