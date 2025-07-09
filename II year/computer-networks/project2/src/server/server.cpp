#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <signal.h>


#include "initialize_socket.h"
#include "command_line_parser.h"
#include "../common/common.h"
#include "../common/err.h"
#include "client_handler.h"
#include "game.h"

int main(int argc, char* argv[]) {
    command_line_parser parser;
    server_config config = parser.parse(argc, argv);
    
    game game;
    game.load_from_file(config.game_file);

    int socket_fd = init_socket(config.port);

    int shutdown_pipe[2];
    if (pipe(shutdown_pipe) != 0) {
        error("pipe failed");
    }

    signal(SIGPIPE, SIG_IGN);
    accept_clients(socket_fd, game, config.timeout, shutdown_pipe);

    close(socket_fd);
    close(shutdown_pipe[0]);
    close(shutdown_pipe[1]);

    return 0;
}
