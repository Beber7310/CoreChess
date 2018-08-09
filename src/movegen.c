/*
 * movegen.c
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "movegen.h"

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

U64 getBishopAttacks(int square, U64 blockers) {
	U64 m1, m2, m3, m4, mr;

	m1 = getPositiveRayAttack(NORTH_WEST, square, blockers);
	m2 = getPositiveRayAttack(NORTH_EAST, square, blockers);
	m3 = getNegativeRayAttack(SOUTH_WEST, square, blockers);
	m4 = getNegativeRayAttack(SOUTH_EAST, square, blockers);
	mr = m1 | m2 | m3 | m4;
	return mr;
}

U64 getRookAttacks(int square, U64 blockers) {
	return getPositiveRayAttack(NORTH, square, blockers) | getPositiveRayAttack(EAST, square, blockers) | getNegativeRayAttack(SOUTH, square, blockers) | getNegativeRayAttack(WEST, square, blockers);
}

/**
 * @brief Returns the index of the LSB in the given bitboard.
 *
 * @param  board Bitboard to get LSB of
 * @return The index of the LSB in the given bitboard.
 */
int _bitscanForward(U64 board) {
	return __builtin_ffsll(board) - 1;
}

/**
 * @brief Returns the index of the MSB in the given bitboard.
 *
 * @param  board Bitboard to get MSB of
 * @return The index of the MSB in the given bitboard.
 */
int _bitscanReverse(U64 board) {
	return __builtin_clzll(board) + 1;
}

U64 getNonSlidingAttacks(uint32_t pieceType, uint32_t square, uint32_t color) {
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
	}
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

/**
 * @brief Moves all set bits in the given bitboard one square east and returns the new bitboard, discarding those that fall off the edge.
 * @param  board Board to move bits east on.
 * @return A bitboard with all set bits moved one square east, bits falling off the edge discarded
 */
U64 _eastOne(U64 board) {
	return ((board << ONE) & (~FILE_A));
}

/**
 * @brief Moves all set bits in the given bitboard one square west and returns the new bitboard, discarding those that fall off the edge.
 * @param  board Board to move bits west on.
 * @return A bitboard with all set bits moved one square west, bits falling off the edge discarded
 */
U64 _westOne(U64 board) {
	return ((board >> ONE) & (~FILE_H));
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
