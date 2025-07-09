#ifndef DEAL_H
#define DEAL_H

#include <vector>
#include <string>
#include <memory>


#include "../common/player.h"
#include "../common/common.h"
#include "trick.h"
#include "../common/err.h"


class deal {
private:
    int deal_type;
    char lead_player;
    std::vector<player> players;
    std::vector<std::shared_ptr<trick>> tricks;
    std::vector<int> scores = std::vector<int>(4);

public:
    deal(int type, char lead);
    void add_player_cards(size_t player_index, const std::string& cards);
    std::shared_ptr<trick> get_current_trick(size_t index);
    std::vector<int> get_scores();
    char get_lead_player();
    player get_player(int seat);
    int get_deal_type();
    void calculate_points();
};

#endif // DEAL_H
