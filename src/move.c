/*
 * move.c
 *
 *  Created on: 12 ao�t 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "move.h"
#include "bitutils.h"

void moveBuild(unsigned int from, unsigned int to, PieceType piece) {
	unsigned int flags=0;
	_move = ((flags  & 0x7f) << 21) | ((to & 0x3f) << 15) | ((from & 0x3f) << 9) | (piece & 0x7);
	_value = 0;
}

void moveBuildCapture(unsigned int from, unsigned int to, PieceType piece, PieceType pieceKilled) {
	moveBuild(from,to,piece);
	unsigned int flags=CAPTURE;
	_move |= ((flags  & 0x7f) << 21)| ((pieceKilled & 0x7)<<6);

}
