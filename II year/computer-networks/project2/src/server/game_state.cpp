#include "game_state.h"

size_t game_state::get_current_deal() {
    return current_deal;
}

size_t game_state::get_current_trick() {
    return current_trick;
}

void game_state::set_deal(size_t deal_number) {
    current_deal = deal_number;
}

void game_state::set_trick(size_t trick_number) {
    current_trick = trick_number;
}

void game_state::set_seat(bool is_occupied) {
    seat_occupied = is_occupied;
}

bool game_state::get_seat() {
    return seat_occupied;
}

void game_state::reset_trick() {
    current_trick = 0;
}

void game_state::set_disconnect_on_deal_handler(bool disconnect) {
    disconnect_on_deal_handler = disconnect;
}

void game_state::set_disconnect_on_trick_handler(bool disconnect) {
    disconnect_on_trick_handler = disconnect;
}

void game_state::set_disconnect_on_send_taken_msg(bool disconnect) {
    disconnect_on_send_taken_msg = disconnect;
}

void game_state::set_disconnect_on_score_total_handler(bool disconnect) {
    disconnect_on_score_total_handler = disconnect;
}


bool game_state::get_disconnect_on_deal_handler() {
    return disconnect_on_deal_handler;
}

bool game_state::get_disconnect_on_trick_handler() {
    return disconnect_on_trick_handler;
}

bool game_state::get_disconnect_on_send_taken_msg() {
    return disconnect_on_send_taken_msg;
}

bool game_state::get_disconnect_on_score_total_handler() {
    return disconnect_on_score_total_handler;
}
