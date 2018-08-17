/*
 * movegen.c
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "movegen.h"
#include "bitutils.h"

U64 NON_SLIDING_ATTACKS[2][6][64] = { { { 0 } } };
U64 RAYS[8][64] = { { 0 } };

void moveGenInit() {
	initPawnAttacks();
	initKnightAttacks();
	initKingAttacks();

	initNorthRays();
	initSouthRays();
	initEastRays();
	initWestRays();

	initNorthEastRays();
	initNorthWestRays();
	initSouthEastRays();
	initSouthWestRays();
}

void getWhitePawnAttacks(sboard* board, int square, smoveList* moveList) {
	U64 m = getNonSlidingAttacks(PAWN, square, WHITE);
	U64 regular = m & ~RANK_8 & board->_allPieces[BLACK];
	U64 prom = m & RANK_8 & board->_allPieces[BLACK];
	U64 enpassant = m & board->_enPassant;

	boardAddMoves(board, moveList, square, PAWN, regular, board->_allPieces[BLACK]);

	boardAddMovesPromotion(board, moveList, square, PAWN, prom, board->_allPieces[BLACK]);

	boardAddMovesEnPassant(board, moveList, square, PAWN, enpassant);
}

void getBlackPawnAttacks(sboard* board, int square, smoveList* moveList) {
	U64 m = getNonSlidingAttacks(PAWN, square, BLACK);
	U64 regular = m & ~RANK_1 & board->_allPieces[WHITE];
	U64 prom = m & RANK_1 & board->_allPieces[WHITE];
	U64 enpassant = m & board->_enPassant;

	boardAddMoves(board, moveList, square, PAWN, regular, board->_allPieces[WHITE]);

	boardAddMovesPromotion(board, moveList, square, PAWN, prom, board->_allPieces[WHITE]);

	boardAddMovesEnPassant(board, moveList, square, PAWN, enpassant);
}

void getWhitePawnMove(sboard* board, int square, smoveList* moveList) {
	U64 pos = ONE << square;

	if (pos & ~(RANK_8 | RANK_7)) {
		if ((board->_occupied & (pos << 8)) == ZERO) {
			moveBuild(&moveList->_sMoveList[moveList->_nbrMove++], square, square + 8, PAWN);
			if (pos & RANK_2) {
				if ((board->_occupied & (pos << 16)) == ZERO) {
					moveBuildDoublePawn(&moveList->_sMoveList[moveList->_nbrMove++], square, square + 16, PAWN);
				}
			}
		}
	} else if ((board->_occupied & (pos << 8)) == ZERO) {

		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], square, square + 8, PAWN, 0, QUEEN);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], square, square + 8, PAWN, 0, ROOK);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], square, square + 8, PAWN, 0, BISHOP);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], square, square + 8, PAWN, 0, KNIGHT);

	}

}

void getBlackPawnMove(sboard* board, int square, smoveList* moveList) {
	U64 pos = ONE << square;

	if (pos & ~(RANK_1 | RANK_2)) {
		if ((board->_occupied & (pos >> 8)) == ZERO) {
			moveBuild(&moveList->_sMoveList[moveList->_nbrMove++], square, square - 8, PAWN);
			if (pos & RANK_7) {
				if ((board->_occupied & (pos >> 16)) == ZERO) {
					//moveBuild(&moveList->_sMoveList[moveList->_nbrMove++], square, square - 16, PAWN);
					moveBuildDoublePawn(&moveList->_sMoveList[moveList->_nbrMove++], square, square - 16, PAWN);
				}
			}
		}
	} else if ((board->_occupied & (pos >> 8)) == ZERO) {

		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], square, square - 8, PAWN, 0, QUEEN);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], square, square - 8, PAWN, 0, ROOK);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], square, square - 8, PAWN, 0, BISHOP);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], square, square - 8, PAWN, 0, KNIGHT);

	}

}

U64 getBishopAttacks(int square, U64 blockers) {
	return getPositiveRayAttack(NORTH_WEST, square, blockers) | getPositiveRayAttack(NORTH_EAST, square, blockers) | getNegativeRayAttack(SOUTH_WEST, square, blockers)
			| getNegativeRayAttack(SOUTH_EAST, square, blockers);
}

U64 getRookAttacks(int square, U64 blockers) {
	return getPositiveRayAttack(NORTH, square, blockers) | getPositiveRayAttack(EAST, square, blockers) | getNegativeRayAttack(SOUTH, square, blockers) | getNegativeRayAttack(WEST, square, blockers);
}

U64 getQueenAttacks(int square, U64 blockers) {
	return getPositiveRayAttack(NORTH_WEST, square, blockers) | getPositiveRayAttack(NORTH_EAST, square, blockers) | getNegativeRayAttack(SOUTH_WEST, square, blockers)
			| getNegativeRayAttack(SOUTH_EAST, square, blockers) | getPositiveRayAttack(NORTH, square, blockers) | getPositiveRayAttack(EAST, square, blockers)
			| getNegativeRayAttack(SOUTH, square, blockers) | getNegativeRayAttack(WEST, square, blockers);

}

U64 getKnightAttacks(int square) {
	return getNonSlidingAttacks(KNIGHT, square, WHITE); // attack are not color dependent
}

U64 getKingAttacks(int square) {
	return getNonSlidingAttacks(KING, square, WHITE); // attack are not color dependent
}

U64 getNonSlidingAttacks(PieceType pieceType, uint32_t square, uint32_t color) {
	return NON_SLIDING_ATTACKS[color][pieceType][square];
}

U64 getSlidingAttacks(PieceType pieceType, int square, U64 blockers) {
	switch (pieceType) {
	case BISHOP:
		return getBishopAttacks(square, blockers);
	case ROOK:
		return getRookAttacks(square, blockers);
	case QUEEN:
		return getBishopAttacks(square, blockers) | getRookAttacks(square, blockers);
	case PAWN:
	case KNIGHT:
	case KING:
		printf("Error, should never reach this code\n");
		return 0;
	}
	printf("Error, should never reach this code\n");
	return 0;
}

U64 getPositiveRayAttack(Dir dir, int square, U64 blockers) {
	U64 attacks = RAYS[dir][square];
	U64 blocked = attacks & blockers;

	if (blocked) {
		int blockSquare = _bitscanForward(blocked);
		attacks ^= RAYS[dir][blockSquare];
	}

	return attacks;
}

U64 getNegativeRayAttack(Dir dir, int square, U64 blockers) {
	U64 attacks = RAYS[dir][square];
	U64 blocked = attacks & blockers;

	if (blocked) {
		int blockSquare = 64 - _bitscanReverse(blocked);
		attacks ^= RAYS[dir][blockSquare];
	}
	return attacks;
}

void initPawnAttacks() {
	for (int i = 0; i < 64; i++) {
		U64 start = ONE << i;

		U64 whiteAttackBb = ((start << 9) & ~FILE_A) | ((start << 7) & ~FILE_H);
		U64 blackAttackBb = ((start >> 9) & ~FILE_H) | ((start >> 7) & ~FILE_A);

		NON_SLIDING_ATTACKS[WHITE][PAWN][i] = whiteAttackBb;
		NON_SLIDING_ATTACKS[BLACK][PAWN][i] = blackAttackBb;
	}
}

void initKnightAttacks() {
	for (int i = 0; i < 64; i++) {
		U64 start = ONE << i;

		U64 attackBb = (((start << 15) | (start >> 17)) & ~FILE_H) | // Left 1
				(((start >> 15) | (start << 17)) & ~FILE_A) | // Right 1
				(((start << 6) | (start >> 10)) & ~(FILE_G | FILE_H)) | // Left 2
				(((start >> 6) | (start << 10)) & ~(FILE_A | FILE_B)); // Right 2

		NON_SLIDING_ATTACKS[WHITE][KNIGHT][i] = attackBb;
		NON_SLIDING_ATTACKS[BLACK][KNIGHT][i] = attackBb;
	}
}

void initKingAttacks() {
	for (int i = 0; i < 64; i++) {
		U64 start = ONE << i;

		U64 attackBb = (((start << 7) | (start >> 9) | (start >> 1)) & (~FILE_H)) | (((start << 9) | (start >> 7) | (start << 1)) & (~FILE_A)) | ((start >> 8) | (start << 8));

		NON_SLIDING_ATTACKS[WHITE][KING][i] = attackBb;
		NON_SLIDING_ATTACKS[BLACK][KING][i] = attackBb;
	}
}

void initNorthRays() {
	U64 north = 0x0101010101010100ull;
	for (int square = 0; square < 64; square++, north <<= 1) {
		RAYS[NORTH][square] = north;
	}
}

void initEastRays() {
	for (int square = 0; square < 64; square++) {
		RAYS[EAST][square] = 2 * ((ONE << (square | 7)) - (ONE << square));
	}
}

void initNorthEastRays() {
	U64 startRay = 0x8040201008040200ull;
	for (int file = 0; file < 8; file++, startRay = _eastOne(startRay)) {
		U64 currRay = startRay;
		for (int rank8 = 0; rank8 < 64; rank8 += 8, currRay <<= 8ull) {
			RAYS[NORTH_EAST][rank8 + file] = currRay;
		}
	}
}

void initNorthWestRays() {
	U64 startRay = 0x102040810204000ull;
	for (int file = 7; file >= 0; file--, startRay = _westOne(startRay)) {
		U64 currRay = startRay;
		for (int rank8 = 0; rank8 < 64; rank8 += 8, currRay <<= 8ull) {
			RAYS[NORTH_WEST][rank8 + file] = currRay;
		}
	}
}

void initSouthEastRays() {
	U64 startRay = 0x2040810204080ull;
	for (int file = 0; file < 8; file++, startRay = _eastOne(startRay)) {
		U64 currRay = startRay;
		for (int rank8 = 56; rank8 >= 0; rank8 -= 8, currRay >>= 8ull) {
			RAYS[SOUTH_EAST][rank8 + file] = currRay;
		}
	}
}

void initSouthRays() {
	U64 south = 0x0080808080808080ull;
	for (int square = 63; square >= 0; square--, south >>= 1) {
		RAYS[SOUTH][square] = south;
	}
}

void initSouthWestRays() {
	U64 startRay = 0x40201008040201ull;
	for (int file = 7; file >= 0; file--, startRay = _westOne(startRay)) {
		U64 currRay = startRay;
		for (int rank8 = 56; rank8 >= 0; rank8 -= 8, currRay >>= 8ull) {
			RAYS[SOUTH_WEST][rank8 + file] = currRay;
		}
	}
}

void initWestRays() {
	for (int square = 0; square < 64; square++) {
		RAYS[WEST][square] = (ONE << square) - (ONE << (square & 56));
	}
}
