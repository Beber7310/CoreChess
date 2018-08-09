/*
 * board.c
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "movegen.h"
#include "bitutils.h"

void boardInit(sboard * pBoard) {
	pBoard->_pieces[WHITE][PAWN] = RANK_2;
	pBoard->_pieces[BLACK][PAWN] = RANK_7;

	pBoard->_pieces[WHITE][ROOK] = RANK_1 & (FILE_A | FILE_H);
	pBoard->_pieces[BLACK][ROOK] = RANK_8 & (FILE_A | FILE_H);

	pBoard->_pieces[WHITE][KNIGHT] = RANK_1 & (FILE_B | FILE_G);
	pBoard->_pieces[BLACK][KNIGHT] = RANK_8 & (FILE_B | FILE_G);

	pBoard->_pieces[WHITE][BISHOP] = RANK_1 & (FILE_C | FILE_F);
	pBoard->_pieces[BLACK][BISHOP] = RANK_8 & (FILE_C | FILE_F);

	pBoard->_pieces[WHITE][QUEEN] = RANK_1 & (FILE_D);
	pBoard->_pieces[BLACK][QUEEN] = RANK_8 & (FILE_D);

	pBoard->_pieces[WHITE][KING] = RANK_1 & (FILE_E);
	pBoard->_pieces[BLACK][KING] = RANK_8 & (FILE_E);

	pBoard->_allPieces[WHITE] = pBoard->_pieces[WHITE][PAWN] | pBoard->_pieces[WHITE][ROOK] | pBoard->_pieces[WHITE][KNIGHT] | pBoard->_pieces[WHITE][BISHOP] | pBoard->_pieces[WHITE][QUEEN]
			| pBoard->_pieces[WHITE][KING];
	pBoard->_allPieces[BLACK] = pBoard->_pieces[BLACK][PAWN] | pBoard->_pieces[BLACK][ROOK] | pBoard->_pieces[BLACK][KNIGHT] | pBoard->_pieces[BLACK][BISHOP] | pBoard->_pieces[BLACK][QUEEN]
			| pBoard->_pieces[BLACK][KING];

	pBoard->_occupied = pBoard->_allPieces[WHITE] | pBoard->_allPieces[BLACK];
	pBoard->_notOccupied = ~pBoard->_occupied;
}

void boardGenerateMove(sboard* board, Color color) {
	PieceType ptype = 0;

	while (ptype < 6) {
		U64 ptm = board->_pieces[color][ptype];
		while (ptm) {
			int from = _popLsb(&ptm);
			U64 m=getAttacksForSquare(board,ptype,color,from);
//		_moves.push_back(Move(to-16, to, PAWN, Move::DOUBLE_PAWN_PUSH));
		}
	}
}

U64 getAttacksForSquare(sboard * pBoard, PieceType pieceType, Color color, int square) {
// Special case for pawns
	if (pieceType == PAWN) {
		switch (color) {
		case WHITE:
			return getWhitePawnAttacks(square) & pBoard->_allPieces[BLACK];
		case BLACK:
			return getBlackPawnAttacks(square) & pBoard->_allPieces[WHITE];
		}
	}

	U64 own = pBoard->_allPieces[color];

	U64 attacks;
	switch (pieceType) {
	case ROOK:
		attacks = getRookAttacks(square, pBoard->_occupied);
		break;
	case KNIGHT:
		attacks = getKnightAttacks(square);
		break;
	case BISHOP:
		attacks = getBishopAttacks(square, pBoard->_occupied);
		break;
	case QUEEN:
		attacks = getQueenAttacks(square, pBoard->_occupied);
		break;
	case KING:
		attacks = getKingAttacks(square);
		break;

	case PAWN:
		printf("Error, should be never reach");
		break;
	}

	return attacks & ~own;
}

void boardPrintMove(U64 m) {
	printf(" ABCDEFGH\n");
	for (int jj = 56; jj >= 0; jj -= 8) {
		printf("%i", 1 + (jj / 8));
		for (int ii = 0; ii < 8; ii++) {
			if (m >> (ii + jj) & ONE)
				printf(".");
			else
				printf(" ");
		}
		printf("\n");
	}
	printf(" ABCDEFGH\n");
}
