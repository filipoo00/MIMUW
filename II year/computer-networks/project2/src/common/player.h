#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <regex>

#include "common.h"
#include "card_values.h"

class player {
private:
    std::map<char, std::vector<std::string>> cards_by_suit;
    std::string cards;
    std::string cards_from_trick;

    //non_auto_player
    std::string played_card;
    std::vector<std::string> available_cards;
    std::vector<std::vector<std::string>> taken_tricks;
    std::vector<std::string> current_trick;

public:
    player();

    int add_cards(const std::string& card);
    const std::vector<std::string>& get_cards_by_suit(char suit);
    void set_cards(const std::string& cards_from_file);
    std::string get_cards();
    void set_cards_from_trick(std::string cards);
    std::string select_card();
    void erase_from_cards_by_suit(std::string card);


    //non_auto_player
    int set_played_card(std::string card);
    std::string get_played_card();
    std::vector<std::string> get_available_cards();
    std::vector<std::vector<std::string>> get_taken_tricks();
    void add_taken_trick(std::vector<std::string> cards_vector);
    std::vector<std::string> get_current_trick();
    void set_current_trick(std::vector<std::string> c_t);
    int check_if_correct_card(std::string card);
    int check_if_exists_suit(char suit);
    void erase_taken_tricks();
    void erase_from_available_cards(std::string card);
};

#endif // PLAYER_H
