#include "player.h"

player::player() {
    cards_by_suit['C'];
    cards_by_suit['D'];
    cards_by_suit['H'];
    cards_by_suit['S'];
}

int player::add_cards(const std::string& cards) {
    std::regex card_pattern(R"((10|[2-9JQKA])([CDHS]))");
    std::smatch matches;

    int count = 0;
    std::string::const_iterator searchStart(cards.cbegin());
    while (std::regex_search(searchStart, cards.cend(), 
                                matches, card_pattern)) {

        std::string card_value = matches[1];

        auto it = card_values.find(card_value);
        if (it == card_values.end()) {
            return -1;
        }

        char suit = matches[2].str()[0];
        if (network_utility::check_suit(suit) < 0) {

            return -1;
        }

        std::string card = card_value + suit;

        cards_by_suit[suit].push_back(card_value);
        available_cards.push_back(card);
        searchStart = matches.suffix().first;
        count++;
    }
    
    if (count != 13) {
        return -1;
    }
    
    return 0;
}

const std::vector<std::string>& player::get_cards_by_suit(char suit) {
    return cards_by_suit[suit];
}

void player::set_cards(const std::string& cards_from_file) {
    cards = cards_from_file;
}

std::string player::get_cards() {
    return cards;
}

void player::set_cards_from_trick(std::string cards) {
    cards_from_trick = cards;
}



std::string player::select_card() {
    std::string first_card = 
                network_utility::extract_first_card(cards_from_trick);

    if (!first_card.empty()) {
        char required_suit = first_card.back();

        if (!cards_by_suit[required_suit].empty()) {
            std::string selected_card = 
                            cards_by_suit[required_suit].front();
            selected_card += required_suit;
            cards_by_suit[required_suit].erase(
                                cards_by_suit[required_suit].begin());
            return selected_card;
        }
    }

    for (auto& suit_cards : cards_by_suit) {
        if (!suit_cards.second.empty()) {
            std::string selected_card = suit_cards.second.front();
            selected_card += suit_cards.first;
            suit_cards.second.erase(suit_cards.second.begin());
            return selected_card;
        }
    }

    return "";
}


int player::set_played_card(std::string card) {
    auto it = std::find(available_cards.begin(), 
                        available_cards.end(), card);
    if (it != available_cards.end()) {
        played_card = card;
        available_cards.erase(it);
        return 0;
    }

    return -1;
}

std::string player::get_played_card() {
    return played_card;
}

std::vector<std::string> player::get_available_cards() {
    return available_cards;
}

std::vector<std::vector<std::string>> player::get_taken_tricks() {
    return taken_tricks;
}

void player::add_taken_trick(std::vector<std::string> cards_vector) {
    taken_tricks.push_back(cards_vector);
}

std::vector<std::string> player::get_current_trick() {
    return current_trick;
}

void player::set_current_trick(std::vector<std::string> c_t) {
    current_trick = c_t;
}

int player::check_if_exists_suit(char suit) {
    for (size_t i = 0; i < available_cards.size(); i++) {
        if (available_cards[i].back() == suit) {
            return -1;
        }
    }

    return 0;
}

int player::check_if_correct_card(std::string card) {
    if (current_trick.size() == 0) {
        return 0;
    }

    std::string first_card = current_trick[0];
    char suit_first_card = first_card.back();
    
    char suit_card = card.back();

    if (suit_card == suit_first_card) {
        return 0;
    }

    return check_if_exists_suit(suit_first_card);
}

void player::erase_taken_tricks() {
    taken_tricks.erase(taken_tricks.begin(), taken_tricks.end());
}

void player::erase_from_cards_by_suit(std::string card) {
    char suit = card.back();
    card.pop_back();
    std::vector<std::string> cards = cards_by_suit[suit];
    auto it = std::remove(cards.begin(), cards.end(), card);
    if (it != cards.end()) {
        cards.erase(it, cards.end());
        cards_by_suit[suit] = cards;
    }
}

void player::erase_from_available_cards(std::string card) {
    auto it = std::remove(available_cards.begin(), 
                            available_cards.end(), card);

    if (it != available_cards.end()) {
        available_cards.erase(it, available_cards.end());
    }
}
