/*
 * movegen.h
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */

#ifndef SRC_MOVEGEN_H_
#define SRC_MOVEGEN_H_

typedef enum {
	NORTH, SOUTH, EAST, WEST, NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST
} Dir;

void moveGenInit(void);


void initPawnAttacks();
void initKnightAttacks();
void initPawnMove();
void initKingAttacks();
void initNorthRays();
void initEastRays();
void initNorthEastRays();
void initNorthWestRays();
void initSouthEastRays();
void initSouthRays();
void initSouthWestRays();
void initWestRays();

U64 getBishopAttacks(int square, U64 blockers);
U64 getRookAttacks(int square, U64 blockers);
U64 getQueenAttacks(int square, U64 blockers);
void getWhitePawnAttacks(sboard* board,int square,smoveList* moveList);
void getBlackPawnAttacks(sboard* board,int square,smoveList* moveList);
void getWhitePawnMove(sboard* board,int square,smoveList* moveList);
void getBlackPawnMove(sboard* board,int square,smoveList* moveList);
U64 getKnightAttacks(int square);
U64 getKingAttacks(int square);

U64 getNonSlidingAttacks(PieceType pieceType, uint32_t square, uint32_t color);
U64 getPositiveRayAttack(Dir dir, int square, U64 blockers);
U64 getNegativeRayAttack(Dir dir, int square, U64 blockers);

#endif /* SRC_MOVEGEN_H_ */
