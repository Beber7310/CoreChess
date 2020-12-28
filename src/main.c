/*
 * main.c
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */
//#include "vld.h"


#include "board.h"
#include "movegen.h"
#include "perft.h"
#include "uci.h"
#include "search.h"
#include "moveOrder.h"
#include "zobrist.h"
#include "transposition.h"

int main_TCP(void);



int main(int argc, char* argv[]) {
	moveGenInit();
	zobInit();
	moveOrderClearKiller();
	ttInit(512*1024*1024);
	initBook();


	FILE * pFile;
	pFile = fopen("logUci.txt", "w");
	fprintf(pFile, " --- Core Chess --- \n");
	for (int ii = 0; ii < argc; ii++)
		fprintf(pFile, "%s\n", argv[ii]);
	fprintf(pFile, ">UCI Start \n");
	fclose(pFile);


	main_UCI();
	//main_TCP();

	return 0;
}


// position startpos moves e2e4 b7b6 d1f3 b8c6 e4e5 c8b7 e5e6 f7e6 b1a3 e6e5 f3h5 g7g6 h5f3 e7e6 a3b5 d7d5 b2b3 a7a6 f3g4 a6b5 g4e6 d8e7 e6d5 g8f6 d5f3 e5e4 f3h3 c6b4 e1d1 e7c5 c2c3 b4d3 f1d3 e4d3 g1f3 f6e4 h3d7 e8d7 h1g1 e4f2 d1e1 a8e8
