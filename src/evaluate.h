/*
 * evaluate.h
 *
 *  Created on: 22 ao�t 2018
 *      Author: Bertrand
 */

#ifndef SRC_EVALUATE_H_
#define SRC_EVALUATE_H_

#define PAWN_VALUE   100
#define KNIGHT_VALUE 320
#define BISHOP_VALUE 330
#define ROOK_VALUE   500
#define QUEEN_VALUE  900
#define INF (99999)

#define BISHOP_PAIR  30;
#define KNIGHT_PAIR  8;
#define ROOK_PAIR  16;

#define MOBILITY_BONUS	2
#define ATTACK_BONUS	5

int evaluate(sboard * pboard);

#endif /* SRC_EVALUATE_H_ */
