/*
 * perft.c
 *
 *  Created on: 15 août 2018
 *      Author: Bertrand
 */

#include "perft.h"

void perft(sboard* pBoard, int* pCnt, int depth) {
	smoveList mList;
	sboard nextBoard;

	if (depth == 0) {
		(*pCnt)++;
		return;
	}
	moveListInit(&mList);
	boardGenerateAllMoves(pBoard, &mList);
	for (int ii = 0; ii < mList._nbrMove; ii++) {
		boardCpy(&nextBoard, pBoard);
		doMove(&nextBoard, &mList._sMoveList[ii]);

		movePrint( &mList._sMoveList[ii]);

		if (!colorIsInCheck(&nextBoard, !nextBoard._ActivePlayer))
			perft(&nextBoard, pCnt, depth - 1);

	}
}

void perftRun(char* posStart, int depth, int expected) {
	sboard board;
	int cnt = 0;
	boardInitFen(&board, posStart);
	perft(&board, &cnt, depth);

	if (cnt != expected)
		printf("perf %i KO get %i expecting %i\n", depth, cnt, expected);
	else
		printf("perf %i OK get %i expecting %i\n", depth, cnt, expected);

}

void perftMasterRun(char* posStart, int e1, int e2, int e3, int e4, int e5, int e6) {

	printf("perf check %s\n", posStart);

	if (e1)
		perftRun(posStart, 1, e1);
	if (e2)
		perftRun(posStart, 2, e2);
	if (e3)
		perftRun(posStart, 3, e3);
	if (e4)
		perftRun(posStart, 4, e4);
	if (e5)
		perftRun(posStart, 5, e5);
	if (e6)
		perftRun(posStart, 6, e6);
	printf("\n");

}

void perftCheck(void) {

	printf("   --- Start perft check ---  \n\n");
	perftMasterRun("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 20, 400, 8902, 197281, 4865609 , 0);
	perftMasterRun("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 48, 2039, 97862, 4085603, 0, 0);
	perftMasterRun("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 24, 496, 9483, 182838, 0, 0);
	perftMasterRun("4k3/8/8/8/8/8/8/4K2R w K - 0 1", 15, 66, 1197, 7059, 133987, 764643);
	perftMasterRun("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 16, 71, 1287, 7626, 145232, 846648);

}

