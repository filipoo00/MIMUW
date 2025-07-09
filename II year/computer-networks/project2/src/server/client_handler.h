#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <array>
#include <vector>
#include <thread>

#include "game.h"
#include "deal.h"

void accept_clients(int server_socket, game& game, int timeout, int *shutdown_pipe);

#endif // CLIENT_HANDLER_H
