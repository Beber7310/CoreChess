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
	ttInit(32*1024*1024);
	
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
