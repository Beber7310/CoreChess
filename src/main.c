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
	//useless? moveOrderClearKiller();
	ttInit(128);
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


// position startpos moves e2e4 e7e5 g1f3 b8c6 f1c4 f8c5 c2c3 g8f6 d2d3 d7d6 c4b3 e8g8 h2h3 d6d5 e4d5 c6a5 b3a4 d8d5 b2b4 e5e4 d3e4 d5e4 d1e2 f8d8 c1g5 e4f5 a4c2 f5d5 b4c5 d8e8 g5e3 a5c4 c2a4 c8d7 a4b3 c4e3 f2e3 d5c5 f3d4 c5e5 e1g1 c7c5 d4c2 b7b5 a2a3 c5c4 b3a2 f6e4 e2f3 f7f5 f3f4 a7a6 f1d1 e5e7 c2d4 g7g6 d1f1 a8d8 a3a4 g6g5 f4c7 f5f4 d4f3 e4g3 f1e1 d8c8 c7b6 d7c6 b6a6 g5g4 a2c4 b5c4 a6c4 e7f7 c4f7 g8f7 h3g4 c6f3 g2f3 e8e3 e1e3 f4e3 g1h2 g3e2 a1a2 e2f4 b1a3 e3e2 a2a1 f4d3 a3c2 f7f6 c2e1 c8c3 e1d3 c3d3 a1h1 d3d1 h1d1 e2d1q f3f4 d1a4 f4f5 a4g4