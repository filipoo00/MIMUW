#ifndef GAME_H
#define GAME_H

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include "deal.h"
#include "../common/err.h"

class game {
private:
    std::vector<std::shared_ptr<deal>> deals;
    size_t how_many_deals;

public:
    void load_from_file(const std::string& filename);
    std::shared_ptr<deal> get_current_deal(size_t index) const;
    size_t get_how_many_deals();
    
};

#endif // GAME_H
