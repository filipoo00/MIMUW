#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include "client_config.h"
#include <stdexcept>
#include <getopt.h>

class command_line_parser {
public:
    client_config parse(int argc, char* argv[]);
private:
    void validate_config(const client_config& config);
};

#endif // COMMAND_LINE_PARSER_H
