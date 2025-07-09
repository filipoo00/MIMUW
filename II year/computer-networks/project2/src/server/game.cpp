#include "game.h"

void game::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        error("Unable to open a file.");
    }
    
    std::string line;
    int lineCount = 0;
    size_t how_many = 0;

    while (std::getline(file, line)) {
        ++lineCount;
        if (line.empty()) continue;
        if (line.size() < 2) {
            error("Line format incorrect");
        }
        
        int type = stoi(line.substr(0, 1));
        char lead = line[1];
        auto new_deal = std::make_shared<deal>(type, lead);
        how_many++;

        for (size_t i = 0; i < 4; ++i) {
            if (std::getline(file, line)) {
                ++lineCount;
                if (line.empty()) continue;
                new_deal->add_player_cards(i, line);

            } 
            else {
                error("Missing card data");
            }
        }

        deals.push_back(new_deal);
    }
    how_many_deals = how_many;
}



std::shared_ptr<deal> game::get_current_deal(size_t index) const {
    return deals[index];
}


size_t game::get_how_many_deals() {
    return how_many_deals;
}
