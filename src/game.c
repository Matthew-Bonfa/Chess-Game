#include "game.h"
#include <stdio.h>

unsigned short get_piece(int *game_state, unsigned short loc) {
    unsigned short row = loc / BOARD_ROWS;
    unsigned short pos = (loc % BOARD_COLS) * SIZE_BITS;

    return (game_state[row] & ((unsigned int)BIT_MASK << pos)) >> pos;
}

void move_piece(int *game_state, unsigned short loc, unsigned short dest) {
}