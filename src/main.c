/*
 * main.c
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "movegen.h"
#include "perft.h"

int main() {
	moveGenInit();

	//perftRun("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ", 3,564);

	perftCheckFile("perftcheck.epd",5);
}

