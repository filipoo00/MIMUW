#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <iostream>
#include <cstdlib>

class game_state {
private:
    bool seat_occupied = false;
    size_t current_deal = 0;
    size_t current_trick = 0;
    bool disconnect_on_deal_handler = false;
    bool disconnect_on_trick_handler = false;
    bool disconnect_on_send_taken_msg = false;
    bool disconnect_on_score_total_handler = false;
    
public:
    size_t get_current_deal();
    size_t get_current_trick();
    void set_seat(bool is_occupied);
    bool get_seat();
    void set_trick(size_t trick_number);
    void set_deal(size_t deal_number);
    void reset_trick();
    void set_disconnect_on_deal_handler(bool disconnect);
    void set_disconnect_on_trick_handler(bool disconnect);
    void set_disconnect_on_send_taken_msg(bool disconnect);
    void set_disconnect_on_score_total_handler(bool disconnect);
    bool get_disconnect_on_deal_handler();
    bool get_disconnect_on_trick_handler();
    bool get_disconnect_on_send_taken_msg();
    bool get_disconnect_on_score_total_handler();
};

#endif // GAME_STATE_H