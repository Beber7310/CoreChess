/*
 * main.c
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "movegen.h"
#include "perft.h"
#include "uci.h"
#include "search.h"
#include "moveOrder.h"
#include "zobrist.h"
#include "transposition.h"


int main(int argc, char* argv[]) {
	moveGenInit();
	zobInit();
	moveOrderInit();
	ttInit(32*1024*1024);
	//perftRun("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ", 3,564);

	//perftCheckFile("perftcheck.epd", 4);

	FILE * pFile;
	pFile = fopen("logUci.txt", "w");
	fprintf(pFile, " --- Core Chess --- \n");
	for (int ii = 0; ii < argc; ii++)
		fprintf(pFile, "%s\n", argv[ii]);
	fprintf(pFile, ">UCI Start \n");
	fclose(pFile);


	main_UCI();
}

