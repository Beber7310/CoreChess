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

#include "search.h"

#define PRINT_PERFT_MOVE	0
#define BUZZ_SIZE 512

#if PRINT_PERFT_MOVE > 0
#endif

int perft(sboard* pBoard, int* pNodeCnt, int depth, int iteration) {
	smoveList mList;
	sboard nextBoard;

	if (depth == 0) {
		(*pNodeCnt)++;
		return 1;
	}

	moveListInit(&mList);
	boardGenerateAllMoves(pBoard, &mList);
	int legalMoves = 0;
	for (int ii = 0; ii < mList._nbrMove; ii++) {
		if (iteration == 0) {
			legalMoves = 0;
		}
		boardCpy(&nextBoard, pBoard);
		doMove(&nextBoard, &mList._sMoveList[ii]);

		if (!colorIsInCheck(&nextBoard, !nextBoard._ActivePlayer)) {
#if PRINT_PERFT_MOVE > 0
			if (iteration == 0) {
				movePrintShort(&mList._sMoveList[ii]);
			}
#endif
			legalMoves += perft(&nextBoard, pNodeCnt, depth - 1, iteration + 1);
#if PRINT_PERFT_MOVE > 0
			if (iteration == 0) {

				printf(" %i\n", legalMoves);
			}
#endif
		}
	}

	return legalMoves;
}

int perftRun(char* posStart, int depth, int expected) {
	sboard board;
	int cnt = 0;
	boardInitFen(&board, posStart);
#if PRINT_PERFT_MOVE > 0
	boardPrint(&board);
#endif
	perft(&board, &cnt, depth, 0);

	if (cnt != expected) {
		printf("perf %i KO get %i expecting %i\n", depth, cnt, expected);
		return 1;
	} else {
		printf("perf %i OK get %i expecting %i\n", depth, cnt, expected);
		return 0;
	}

}

int perftMasterRun(char* posStart, int e1, int e2, int e3, int e4, int e5, int e6) {
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

int perftCheckFile(char* fileName, int depth) {

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
			D[ii] = strtok(strD[ii], " ");
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

int puzzleMasterRun(char* posStart, int depth, int* nbrNode, int* nbrCut, int* nbrZob) {
	sboard board;
	boardInitFen(&board, posStart);
	boardPrint(&board);
#if PRINT_PERFT_MOVE > 0
	boardPrint(&board);
#endif

	searchStat stat;

	printf("%s", posStart);
	searchStart(&board, 80000, 80000, 10000, 0, &stat);


	if ((stat.boardEval != INF) && (stat.boardEval != -INF)) {
		printf("Error puzzleMasterRun! \n");
		movePrintShort(&board._bestMove);
		printf("\n");
		return 1;
	} else {
		printf("OK! ");
		movePrintShort(&board._bestMove);

 

		printf("\n");
		printf("Terminal node %i\n", stat.nbrNode);
		printf("Cut           %i\n", stat.nbrCut);
		printf("Zob           %i\n\n", stat.nbrZob);

		*nbrNode += stat.nbrNode;
		*nbrCut += stat.nbrCut;
		*nbrZob += stat.nbrZob;
		return 0;
	}

}

int puzzlzCheckFile(char* fileName, int depth) {

	char buff[BUZZ_SIZE];
	char* pos;

	int nbrNode = 0;
	int nbrCut = 0;
	int nbrZob = 0;

	int startTime, endTime;

	int err = 0;
	FILE *f = fopen(fileName, "r");
	if (f == NULL)
		printf("Error while opening puzzle file %s\n", fileName);

	startTime =(int) time(NULL);
	while (fgets(buff, BUZZ_SIZE, f)) {
		pos = strtok(buff, ";");
		err += puzzleMasterRun(pos, depth, &nbrNode, &nbrCut, &nbrZob);
	}
	endTime = (int)  time(NULL);
	fclose(f);

	if (err) {
		printf("Puzzle check finish with error!\n");
	} else {
		printf("   --- Puzzle check OK! --- \n");
		printf("Nodes %i\n", nbrNode);
		if ((endTime - startTime))
			printf("nps   %i\n", nbrNode / (endTime - startTime));
		if ((endTime - startTime))
			printf("time   %i\n", endTime - startTime);
		printf("Cuts  %i\n", nbrCut);
		printf("Zobs  %i\n", nbrZob);

	}
	return 0;
}


int algoCheckMasterRun(char* posStart, int depth, int* nbrNode, int* nbrCut, int* nbrZob) {
	sboard board;
	boardInitFen(&board, posStart);
#if PRINT_PERFT_MOVE > 0
	boardPrint(&board);
#endif

	searchStat stat;

	printf("%s", posStart);
	algoCheck(&board, negamax, alphaBeta,  &stat);


	if ((stat.boardEval != INF) && (stat.boardEval != -INF)) {
		printf("Error puzzleMasterRun! \n");
		movePrintShort(&board._bestMove);
		return 1;
	} else {
		printf("OK! ");
		movePrintShort(&board._bestMove);



		printf("\n");
		printf("Terminal node %i\n", stat.nbrNode);
		printf("Cut           %i\n", stat.nbrCut);
		printf("Zob           %i\n\n", stat.nbrZob);

		*nbrNode += stat.nbrNode;
		*nbrCut += stat.nbrCut;
		*nbrZob += stat.nbrZob;
		return 0;
	}

}


int puzzlzCompAlgo(char* fileName, int depth) {

	char buff[BUZZ_SIZE];
	char* pos;

	int nbrNode = 0;
	int nbrCut = 0;
	int nbrZob = 0;

	int startTime, endTime;

	int err = 0;
	FILE *f = fopen(fileName, "r");
	if (f == NULL)
		printf("Error while opening puzzle file %s\n", fileName);

	startTime =(int) time(NULL);
	while (fgets(buff, BUZZ_SIZE, f)) {
		pos = strtok(buff, ";");
		err += algoCheckMasterRun(pos, depth, &nbrNode, &nbrCut, &nbrZob);
	}
	endTime = (int)  time(NULL);
	fclose(f);

	if (err) {
		printf("Puzzle check finish with error!\n");
	} else {
		printf("   --- Puzzle check OK! --- \n");
		printf("Nodes %i\n", nbrNode);
		if ((endTime - startTime))
			printf("nps   %i\n", nbrNode / (endTime - startTime));
		printf("Cuts  %i\n", nbrCut);
		printf("Zobs  %i\n", nbrZob);

	}
	return 0;
}

