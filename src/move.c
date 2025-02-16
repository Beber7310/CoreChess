/*
 * move.c
 *
 *  Created on: 12 ao�t 2018
 *      Author: Bertrand
 */
#include <string.h>

#include "board.h"
#include "move.h"
#include "bitutils.h"
#include "tcpserver.h"

extern inline void moveBuild(smove* pMove, unsigned int from, unsigned int to, PieceType piece) {
	unsigned int flags = 0;
	pMove->_move = ((flags & 0x7f) << 21) | ((to & 0x3f) << 15) | ((from & 0x3f) << 9) | (piece & 0x7);
	pMove->_value = 0;
}

extern inline void moveBuildDoublePawn(smove* pMove, unsigned int from, unsigned int to, PieceType piece) {
	unsigned int flags = DOUBLE_PAWN_PUSH;
	pMove->_move = ((flags & 0x7f) << 21) | ((to & 0x3f) << 15) | ((from & 0x3f) << 9) | (piece & 0x7);
	pMove->_value = 0;
}


extern inline void moveBuildCapture(smove* pMove, unsigned int from, unsigned int to, PieceType piece, PieceType pieceKilled) {
	moveBuild(pMove, from, to, piece);
	unsigned int flags = CAPTURE;
	pMove->_move |= ((flags & 0x7f) << 21) | ((pieceKilled & 0x7) << 6);
}

extern inline void moveBuildEnPassant(smove* pMove, unsigned int from, unsigned int to, PieceType piece) {
	moveBuild(pMove, from, to, piece);
	unsigned int flags = EN_PASSANT;
	pMove->_move |= ((flags & 0x7f) << 21);
}

extern inline void moveBuildPromotion(smove* pMove, unsigned int from, unsigned int to, PieceType piece, PieceType pieceKilled, PieceType pieceProm) {
	moveBuild(pMove, from, to, piece);
	unsigned int flags = PROMOTION;
	if (pieceKilled)
		flags |= CAPTURE;
	pMove->_move |= ((flags & 0x7f) << 21) | ((pieceKilled & 0x7) << 6) | ((pieceProm & 0x7) << 3);
}

extern inline void moveBuildCastle(smove* pMove, unsigned int from, unsigned int to, int flags) {
	moveBuild(pMove, from, to, KING);
	pMove->_move |= ((flags & 0x7f) << 21);
}

void moveListInit(smoveList* pList) {
	memset(pList, NULL, sizeof(smoveList));
}

void movePrint(smove* move) {
	int from, to;
	from = MOVE_FROM(move->_move);
	to = MOVE_TO(move->_move);

	printf("%c%i%c%i flag %x prom %x\n", 'a' + (from & 0x7), (from >> 3) + 1, 'a' + (to & 0x07), (to >> 3) + 1, MOVE_FLAG(move->_move), MOVE_PIECE_PROMOTION(move->_move));
}

void movePrintShort(smove* move, char* str) {
	int from, to;
	 

	from = MOVE_FROM(move->_move);
	to = MOVE_TO(move->_move);

	sprintf(str, "%c%i%c%i", 'a' + (from & 0x7), (from >> 3) + 1, 'a' + (to & 0x07), (to >> 3) + 1);
	//printTcp(str);

	if (MOVE_FLAG(move->_move) & PROMOTION) {
		switch (MOVE_PIECE_PROMOTION(move->_move)) {
		case QUEEN:strcat(str,"q");
			break;
		case ROOK: strcat(str, "r");
			break;
		case KNIGHT: strcat(str, "n");
			break;
		case BISHOP: strcat(str, "b");
			break;
		default:
			break;
		}
	}
	strcat(str, " ");
	//printTcp(" ");
}

void moveBreakPoint(smove* move, char* target) {
	int from, to;
	char str[10];


	from = MOVE_FROM(move->_move);
	to = MOVE_TO(move->_move);

	sprintf(str, "%c%i%c%i", 'a' + (from & 0x7), (from >> 3) + 1, 'a' + (to & 0x07), (to >> 3) + 1);
	if (strcmp(str, target) == 0)
	{
		printf("moveBreakPoint find move %s\n", target);
	}


}

 extern inline void moveCpy(smove* dst, smove* src) {
	*dst = *src;
}

