/*
 * board.h
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */

#ifndef SRC_BOARD_H_
#define SRC_BOARD_H_

#include "defs.h"

typedef struct sboard_t
{
/**
 * @brief Array indexed by [color][piecetype] of piece bitboards
 */
U64 _pieces[2][6];

/**
 * @brief Array indexed by [color] of bitboards containing all pieces of each color.
 */
U64 _allPieces[2];

/**
 * @brief Array indexed by [color] of bitboards containing all attackable pieces.
 */
U64 _attackable[2];

/**
 * @brief Bitboard containing all occupied squares.
 */
U64 _occupied;

/**
 * @brief Bitboard containing all unoccupied squares.
 */
U64 _notOccupied;


} sboard;

void boardPrintMove(U64 m);

#endif /* SRC_BOARD_H_ */
