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
#include "book.h"

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


	//main_UCI();
	main_TCP();

	return 0;
}

 //Bug possible -->  position startpos moves d2d4 g8f6 c2c4 e7e6 b1c3 d7d5 c4d5 e6d5 c1g5 c8g4 h2h3 g4e6 d1b3 c7c5 b3b7 h7h6 g5f6 g7f6 b7a8 c5d4 a8a7 d4c3 b2c3 b8c6 a7b7 e6d7 a1d1 d5d4 b7b1 d8a5 b1d3 a5a3 g2g4 a3c3 d3c3 d4c3 f1g2 c3c2 d1c1 f8b4 e1f1 c6d4 g2e4 f6f5 e4c2 b4a3 c1b1 e8g8 c2f5 d4f5 g4f5 d7f5 b1b3 f5e4 f2f3 f7f6 f3e4 a3d6 b3b6 d6c5 b6c6 c5e3 c6c3 e3d2 c3d3 d2f4 g1f3 f8a8 h1g1 g8f8 e2e3 f4g5 f3g5 h6g5 
// position startpos moves e2e4 e7e5 g1f3 b8c6 f1c4 f8c5 c2c3 g8f6 d2d3 d7d6 c4b3 e8g8 h2h3 d6d5 e4d5 c6a5 b3a4 d8d5 b2b4 e5e4 d3e4 d5e4 d1e2 f8d8 c1g5 e4f5 a4c2 f5d5 b4c5 d8e8 g5e3 a5c4 c2a4 c8d7 a4b3 c4e3 f2e3 d5c5 f3d4 c5e5 e1g1 c7c5 d4c2 b7b5 a2a3 c5c4 b3a2 f6e4 e2f3 f7f5 f3f4 a7a6 f1d1 e5e7 c2d4 g7g6 d1f1 a8d8 a3a4 g6g5 f4c7 f5f4 d4f3 e4g3 f1e1 d8c8 c7b6 d7c6 b6a6 g5g4 a2c4 b5c4 a6c4 e7f7 c4f7 g8f7 h3g4 c6f3 g2f3 e8e3 e1e3 f4e3 g1h2 g3e2 a1a2 e2f4 b1a3 e3e2 a2a1 f4d3 a3c2 f7f6 c2e1 c8c3 e1d3 c3d3 a1h1 d3d1 h1d1 e2d1q f3f4 d1a4 f4f5 a4g4