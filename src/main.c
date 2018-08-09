/*
 * main.c
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */



#include "board.h"
#include "movegen.h"

int main()
{
	U64 m;

	sboard board;

	moveGenInit();

	boardInit(&board);

	m=getBishopAttacks(30, board._occupied);

	boardPrintMove(m);

}
