/*
 * main.c
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "movegen.h"

int main() {
	U64 m;

	sboard board;

	moveGenInit();

	boardInit(&board);

	printf("Beshop\n");
	m = getBishopAttacks(30, board._occupied);
	boardPrintMove(m);

	printf("Rook\n");
	m = getRookAttacks(30, board._occupied);
	boardPrintMove(m);

	printf("Queen\n");
	m = getQueenAttacks(30, board._occupied);
	boardPrintMove(m);

}
