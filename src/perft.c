/*
 * perft.c
 *
 *  Created on: 15 août 2018
 *      Author: Bertrand
 */

#include "perft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINT_PERFT_MOVE	1

int perft(sboard* pBoard, int* pCnt, int depth) {
	smoveList mList;
	sboard nextBoard;

	if (depth == 0) {
		(*pCnt)++;
		return 0;
	}

	moveListInit(&mList);
	boardGenerateAllMoves(pBoard, &mList);
	int legalMoves=0;
	for (int ii = 0; ii < mList._nbrMove; ii++) {
		boardCpy(&nextBoard, pBoard);
		doMove(&nextBoard, &mList._sMoveList[ii]);

		if (!colorIsInCheck(&nextBoard, !nextBoard._ActivePlayer)) {
#if PRINT_PERFT_MOVE > 0
			if (depth == 1) {
				movePrint(&mList._sMoveList[ii]);
			}
#endif
			legalMoves++;
			legalMoves+=perft(&nextBoard, pCnt, depth - 1);
		}
	}
#if PRINT_PERFT_MOVE > 0
	//if (depth == 2)
		printf(" %i\n",legalMoves);
#endif
		return legalMoves;
}

int perftRun(char* posStart, int depth, int expected) {
	sboard board;
	int cnt = 0;
	boardInitFen(&board, posStart);
#if PRINT_PERFT_MOVE > 0
	boardPrint(&board);
#endif
	perft(&board, &cnt, depth);

	if (cnt != expected) {
		printf("perf %i KO get %i expecting %i\n", depth, cnt, expected);
		return 1;
	} else {
		printf("perf %i OK get %i expecting %i\n", depth, cnt, expected);
		return 0;
	}

}

int perftMasterRun(char* posStart, int e1, int e2, int e3, int e4, int e5,
		int e6) {
	int err = 0;
	printf("perf check %s\n", posStart);

	if (e1)
		err += perftRun(posStart, 1, e1);
	if (e2)
		err += perftRun(posStart, 2, e2);
	if (e3)
		err += perftRun(posStart, 3, e3);
	if (e4)
		err += perftRun(posStart, 4, e4);
	if (e5)
		err += perftRun(posStart, 5, e5);
	if (e6)
		err += perftRun(posStart, 6, e6);
	printf("\n");

	return err;

}

void perftCheck(void) {

	printf("   --- Start perft check ---  \n\n");
	perftMasterRun("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
			20, 400, 8902, 197281, 4865609, 0);
	perftMasterRun(
			"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
			48, 2039, 97862, 4085603, 0, 0);
	perftMasterRun("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 24, 496, 9483,
			182838, 0, 0);
	perftMasterRun("4k3/8/8/8/8/8/8/4K2R w K - 0 1", 15, 66, 1197, 7059, 133987,
			764643);
	perftMasterRun("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 16, 71, 1287, 7626, 145232,
			846648);

}

int perftCheckFile(char* fileName, int depth) {
#define BUZZ_SIZE 512
	char buff[BUZZ_SIZE];
	char* pos;
	char* strD[6];
	int D[6];
	int err = 0;
	FILE *f = fopen(fileName, "r");
	if (f == NULL)
		printf("Error while opening perft file %s\n", fileName);

	while (fgets(buff, BUZZ_SIZE, f)) {

		pos = strtok(buff, ";");
		for (int ii = 0; ii < 6; ii++) {
			strD[ii] = strtok(NULL, ";");
		}
		for (int ii = 0; ii < 6; ii++) {
			strtok(strD[ii], " ");
			D[ii] = 0;
			if (ii < depth)
				D[ii] = atoi(strtok(NULL, " "));
		}

		err += perftMasterRun(pos, D[0], D[1], D[2], D[3], D[4], D[5]);
	}
	fclose(f);

	if (err) {
		printf("perft finish with error!\n");
	} else
		printf("perft check OK!\n");

	return 0;
}

