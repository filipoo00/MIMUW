#ifndef TRICK_H
#define TRICK_H

#include <iostream>
#include <vector>
#include "../common/card.h"
#include "../common/common.h"
#include "../common/card_values.h"

class trick {
private:
    int winning_player;
    int current_trick_player;
    std::string cards_string;
    std::vector<card> cards;

public:
    trick(int start_player);
    void add_card_to_cards(std::string& card);
    void set_winning_player(int player);
    int get_winning_player();  
    std::string get_cards_string();
    std::vector<card> get_cards();
};

#endif // TRICK_H
