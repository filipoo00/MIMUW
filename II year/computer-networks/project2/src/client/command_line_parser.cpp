#include <iostream>

#include "command_line_parser.h"
#include "../common/err.h"
#include "../common/common.h"

client_config command_line_parser::parse(int argc, char* argv[]) {
    client_config config;
    config.use_IPv4 = false;
    config.use_IPv6 = false;
    config.is_auto_player = false;
    config.is_port = false;

    int opt;
    while ((opt = getopt(argc, argv, "h:p:46NESWa")) != -1) {
        switch (opt) {
            case 'h':
                config.host = optarg;
                break;
            case 'p':
                size_t port;
                if (!network_utility::safe_stoi(optarg, port)) {
                    error("Port must be a number");
                }
                config.port = port;
                config.is_port = true;
                break;
            case '4':
                config.use_IPv4 = true;
                config.use_IPv6 = false;
                break;
            case '6':
                config.use_IPv6 = true;
                config.use_IPv4 = false;
                break;
            case 'N':
            case 'E':
            case 'S':
            case 'W':
                config.position = static_cast<char>(opt);
                break;
            case 'a':
                config.is_auto_player = true;
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

void command_line_parser::validate_config(const client_config& config) {
    if (config.host.empty()) {
        error("Host (-h) is a mandatory parameter");
    }
    if (config.is_port == false) {
        error("Port (-p) is a mandatory parameter");
    }
    if (config.position != 'N' && config.position != 'E' &&
        config.position != 'S' && config.position != 'W') {
        error("Position (-N, -E, -S, -W) is a mandatory parameter");
    }
}
