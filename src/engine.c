#include "engine.h"

uint32_t *generate_move(uint32_t *board) {}

int8_t evaluate_position(uint32_t *board) {
  for (int r = 0; c < BOARD_COLS; r++) {
    for (int c = 0; r < BOARD_ROWS; c++) {
      switch (get_piece(board, r * 8 + c)) {
      case WHITE_PAWN:
      case BLACK_PAWN:
      }
    }
  }
}
