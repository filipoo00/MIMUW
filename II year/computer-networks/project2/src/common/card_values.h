#ifndef CARD_VALUES_H
#define CARD_VALUES_H

#include <map>
#include <string>

static const std::map<std::string, int> card_values = {
    {"2", 2}, {"3", 3}, {"4", 4}, {"5", 5}, {"6", 6},
    {"7", 7}, {"8", 8}, {"9", 9}, {"10", 10},
    {"J", 11}, {"Q", 12}, {"K", 13}, {"A", 14}
};



#endif // CARD_VALUES_H
