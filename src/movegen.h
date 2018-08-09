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

U64 _eastOne(U64 board);
U64 _westOne(U64 board);

void initPawnAttacks();
void initKnightAttacks();
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

U64 getPositiveRayAttack(Dir dir, int square, U64 blockers);
U64 getNegativeRayAttack(Dir dir, int square, U64 blockers);

int bitscanForward(U64 board);
int bitscanReverse(U64 board);

#endif /* SRC_MOVEGEN_H_ */
