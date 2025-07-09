#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <stdexcept>
#include <getopt.h>

#include "server_config.h"

class command_line_parser {
public:
    server_config parse(int argc, char* argv[]);
private:
    void validate_config(const server_config& config);
};

#endif // COMMAND_LINE_PARSER_H
