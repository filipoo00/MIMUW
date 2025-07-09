#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <string>
#include <iostream>
#include <cstdlib>

[[noreturn]] void error(const std::string& message);

#endif // ERROR_HANDLING_H