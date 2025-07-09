#include <iostream>

#include "deal.h"

deal::deal(int type, char lead) : deal_type(type), lead_player(lead),
                                    players(4) {
    
    for (size_t i = 0; i < 13; i++) {
        tricks.push_back(std::make_shared<trick>(
                            network_utility::char_to_index(lead_player)));
    }                                          
}

void deal::add_player_cards(size_t player_index, const std::string& cards) {
    if (player_index >= players.size()) {
        error("bad player index - file.");
    }
    players[player_index].set_cards(cards);
    players[player_index].add_cards(cards);
}

std::shared_ptr<trick> deal::get_current_trick(size_t index) {
    return tricks[index];
}

std::vector<int> deal::get_scores() {
    return scores;
}

char deal::get_lead_player() {
    return lead_player;
}

player deal::get_player(int seat) {
    return players[seat];
}

int deal::get_deal_type() {
    return deal_type;
}

void deal::calculate_points() {
    for (size_t i = 0; i < tricks.size(); ++i) {
        std::shared_ptr<trick> trick = tricks[i];
        int winner = trick->get_winning_player();

        if (deal_type == 1) {
            scores[winner] += 1;
        } 
        else if (deal_type == 2) {
            for (auto card : trick->get_cards()) {
                if (card.suit == 'H') {
                    scores[winner] += 1;
                }
            }
        } 
        else if (deal_type == 3) {
            for (auto card : trick->get_cards()) {
                if (card.value_string == "Q") {
                    scores[winner] += 5;
                }
            }
        } 
        else if (deal_type == 4) {
            for (auto card : trick->get_cards()) {
                if (card.value_string == "J" || card.value_string == "K") {
                    scores[winner] += 2;
                }
            }
        }
        else if (deal_type == 5) {
            for (auto card : trick->get_cards()) {
                if (card.value_string == "K" && card.suit == 'H') {
                    scores[winner] += 18;
                }
            }
        }
        else if (deal_type == 6) {
            if (i == 6 || i == 12) {
                scores[winner] += 10;
            }
        } 
        else if (deal_type == 7) {
            scores[winner] += 1;
            for (auto card : trick->get_cards()) {
                if (card.suit == 'H') {
                    scores[winner] += 1;
                }
                if (card.value_string == "Q") {
                    scores[winner] += 5;
                }
                if (card.value_string == "J" || card.value_string == "K") {
                    scores[winner] += 2;
                }
                if (card.value_string == "K" && card.suit == 'H') {
                    scores[winner] += 18;
                }
            }
            if (i == 6 || i == 12) {
                scores[winner] += 10;
            }
        }
    }
}
