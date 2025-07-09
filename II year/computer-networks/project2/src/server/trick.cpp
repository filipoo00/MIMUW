#include "trick.h"

trick::trick(int start_player) : winning_player(-1),
                                current_trick_player(start_player) {}


void trick::set_winning_player(int player) {
    winning_player = player;
}


void trick::add_card_to_cards(std::string& c) {
    cards_string += c;

    char suit = c.back();
    std::string value_string = c.substr(0, c.size() - 1);

    card new_card;
    new_card.suit = suit;
    new_card.value_string = value_string;
    auto it = card_values.find(value_string);
    new_card.value_int = it->second;
    cards.push_back(new_card);
}

int trick::get_winning_player() {
    return winning_player;
}

std::string trick::get_cards_string() {
    return cards_string;
}

std::vector<card> trick::get_cards() {
    return cards;
}