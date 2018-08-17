/*
 * move.c
 *
 *  Created on: 12 ao�t 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "move.h"
#include "bitutils.h"

void moveBuild(smove* pMove, unsigned int from, unsigned int to, PieceType piece) {
	unsigned int flags = 0;
	pMove->_move = ((flags & 0x7f) << 21) | ((to & 0x3f) << 15) | ((from & 0x3f) << 9) | (piece & 0x7);
	pMove->_value = 0;
}

void moveBuildDoublePawn(smove* pMove, unsigned int from, unsigned int to, PieceType piece) {
	unsigned int flags = DOUBLE_PAWN_PUSH;
	pMove->_move = ((flags & 0x7f) << 21) | ((to & 0x3f) << 15) | ((from & 0x3f) << 9) | (piece & 0x7);
	pMove->_value = 0;
}


void moveBuildCapture(smove* pMove, unsigned int from, unsigned int to, PieceType piece, PieceType pieceKilled) {
	moveBuild(pMove, from, to, piece);
	unsigned int flags = CAPTURE;
	pMove->_move |= ((flags & 0x7f) << 21) | ((pieceKilled & 0x7) << 6);
}

void moveBuildEnPassant(smove* pMove, unsigned int from, unsigned int to, PieceType piece) {
	moveBuild(pMove, from, to, piece);
	unsigned int flags = EN_PASSANT;
	pMove->_move |= ((flags & 0x7f) << 21);
}

void moveBuildPromotion(smove* pMove, unsigned int from, unsigned int to, PieceType piece, PieceType pieceKilled, PieceType pieceProm) {
	moveBuild(pMove, from, to, piece);
	unsigned int flags = PROMOTION;
	if(pieceKilled)
		flags |= CAPTURE;
	pMove->_move |= ((flags & 0x7f) << 21) | ((pieceKilled & 0x7) << 6) | ((pieceProm&0x7)<<3);
}


void moveBuildCastle(smove* pMove, unsigned int from, unsigned int to,  int flags) {
	moveBuild(pMove, from, to, KING);
	pMove->_move |= ((flags & 0x7f) << 21);
}

void moveListInit(smoveList* pList) {

	for (int ii = 0; ii < (sizeof(pList->_sMoveList) / sizeof(smove)); ii++) {
		pList->_sMoveList[ii]._move = 0;
		pList->_sMoveList[ii]._value = 0;
	}
	pList->_nbrMove = 0;

}

void movePrint(smove* move) {
	int from,to;
	from=MOVE_FROM(move->_move);
	to=MOVE_TO(move->_move);

	printf("%c%i%c%i flag %x prom %x\n",'a'+(from&0x7),(from>>3)+1,'a'+(to&0x07),(to>>3)+1,MOVE_FLAG(move->_move),MOVE_PIECE_PROMOTION(move->_move));
	//printf("%c%i%c%i",'a'+(from&0x7),(from>>3)+1,'a'+(to&0x07),(to>>3)+1);
}
