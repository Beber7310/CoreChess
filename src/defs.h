/*
 * defs.h
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */

#ifndef DEFS_H_
#define DEFS_H_

#include "stdint.h"

typedef unsigned long long U64;


/**
 * @name Rank bitboards
 *
 * @{
 */
#define RANK_1 (0xffull)
#define RANK_2 (0xff00ull)
#define RANK_3 (0xff0000ull)
#define RANK_4 (0xff000000ull)
#define RANK_5 (0xff00000000ull)
#define RANK_6 (0xff0000000000ull)
#define RANK_7 (0xff000000000000ull)
#define RANK_8 (0xff00000000000000ull)
/**@}*/

/**
 * @name File bitboards
 *
 * @{
 */
#define FILE_H (0x8080808080808080ull)
#define FILE_G (0x4040404040404040ull)
#define FILE_F (0x2020202020202020ull)
#define FILE_E (0x1010101010101010ull)
#define FILE_D (0x0808080808080808ull)
#define FILE_C (0x0404040404040404ull)
#define FILE_B (0x0202020202020202ull)
#define FILE_A (0x0101010101010101ull)
/**@}*/

/**
 * @name Black and white squares
 */
#define BLACK_SQUARES (0xAA55AA55AA55AA55ull);
#define WHITE_SQUARES (0x55AA55AA55AA55AAull);
/**@}*/


/**
 * @enum Color
 * @brief Represents a color.
 */
typedef enum {
  WHITE,
  BLACK
}Color;

/**
 * @enum PieceType
 * @brief Represents a piece type.
 */
typedef enum  {
  PAWN,
  ROOK,
  KNIGHT,
  BISHOP,
  QUEEN,
  KING
} PieceType;


/**
 * @enum SquareIndex
 *
 * Little endian rank file mapping of each square.
 */
enum SquareIndex {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8
};

/**
 * @brief An empty bitboard. (ie. the number 0)
 */
#define ZERO  ((U64)0)

/**
 * @brief A bitboard containing only the square a1. (ie. the number 1)
 */
#define ONE  ((U64)1)

#endif /* DEFS_H_ */
