#include <iostream>
#include <cstring>

#include "command_line_parser.h"
#include "../common/err.h"
#include "../common/common.h"

server_config command_line_parser::parse(int argc, char* argv[]) {
    server_config config;

    int opt;
    while ((opt = getopt(argc, argv, "p:f:t:")) != -1) {
        switch (opt) {
            case 'p':
                size_t port;
                if (!network_utility::safe_stoi(optarg, port)) {
                    error("Port must be a number");
                }
                config.port = port;
                break;
            case 'f':
                config.game_file = optarg;
                break;
            case 't':
                config.timeout = std::stoi(optarg);
                break;
            default:
                error("Unknown option");
        }
    }

    for (int index = optind; index < argc; index++) {
        error("bad parameters");
    }

    validate_config(config);
    return config;
}

void command_line_parser::validate_config(const server_config& config) {
    if (config.game_file.empty()) {
        error("Game file (-f) is a mandatory parameter");
    }
    if (config.timeout <= 0) {
        error("Timeout (-t) must be a positive number");
    }
}
