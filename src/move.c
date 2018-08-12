/*
 * move.c
 *
 *  Created on: 12 août 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "move.h"
#include "bitutils.h"

void moveBuild(unsigned int from, unsigned int to, PieceType piece, unsigned int flags) {
	_move = ((flags  & 0x7f) << 21) | ((to & 0x3f) << 15) | ((from & 0x3f) << 9) | (piece & 0x7);
	_value = 0;
}
