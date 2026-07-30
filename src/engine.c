#include "engine.h"
#include <stdint.h>

uint32_t *generate_move(uint32_t *board) {}

uint32_t pawn_heuristic(uint32_t *board, uint8_t color, uint8_t r, uint8_t c) {
    uint32_t c_dist =  10 / (abs(35 - 10 * c));
    uint32_t r_dist = color == WHITE ? 7 - r : r;
    uint32_t eval = PAWN + PAWN_COL_W * c_dist + PAWN_ROW_W * r_dist;
    return color == WHITE ? eval : -1 * eval;
}

uint32_t knight_heuristic(uint32_t *board, uint8_t color, uint8_t r, uint8_t c) {
    uint32_t c_dist =  10 / (abs(35 - 10 * c));
    uint32_t r_dist = 10 / (abs(35 - 10 * r));
    uint32_t eval = KNIGHT + KNIGHT_COL_W * c_dist + KNIGHT_ROW_W * r_dist;
    return color == WHITE ? eval : -1 * eval;
}

uint32_t bishop_heuristic(uint32_t *board, uint8_t color, uint8_t r, uint8_t c) {
    uint32_t main_diag = 8 - abs(r - c);
    uint32_t anti_diag = 8 - abs(r + c - 7);
    uint32_t eval = BISHOP_DIAG_W * (main_diag + anti_diag);
    return color == WHITE ? eval : -1 * eval;
}

uint32_t rook_heuristic(uint32_t *board) {}
uint32_t queen_heuristic(uint32_t *board) {}
uint32_t king_heuristic(uint32_t *board) {}

int32_t evaluate_position(uint32_t *board) {
    for (uint8_t r = 0; r < BOARD_ROWS; r++) {
        for (uint8_t c = 0; c < BOARD_COLS; c++) {
            uint32_t eval = 0;
            switch (get_piece(board, r * 8 + c)) {
            case WHITE_PAWN:
                eval += pawn_heuristic(board, WHITE, r, c);
                break;
            case BLACK_PAWN:
                eval += pawn_heuristic(board, BLACK, r, c);
                break;
            case WHITE_KNIGHT:
                eval += knight_heuristic(board, WHITE, r, c);
                break;
            case BLACK_KNIGHT:
                eval += knight_heuristic(board, BLACK, r, c);
                break;
            case WHITE_BISHOP:
                eval += bishop_heuristic(board, WHITE, r, c);
                break;
            case BLACK_BISHOP:
                eval += bishop_heuristic(board, BLACK, r, c);
                break;
            case WHITE_ROOK:
            case BLACK_ROOK:
                eval += rook_heuristic( board);
                break;
            case WHITE_QUEEN:
            case BLACK_QUEEN:
                eval += queen_heuristic(board);
                break;
            case WHITE_KING:
            case BLACK_KING:
                eval += king_heuristic(board);
                break;
            }
        }
    }
}
