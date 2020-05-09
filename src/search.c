/*
 * search.c
 *
 *  Created on: 21 août 2018
 *      Author: Bertrand
 */

#include <time.h>
#include "bitutils.h"
#include "board.h"
#include "search.h"
#include "evaluate.h"
#include "search.h"
#include "moveOrder.h"
#include "transposition.h"

static int gStopSearch;		  // used to stop digging when time is over
static int gNodeCptCheckTime; //Used as a counter to choose when to print some info

//searchStat stat;

#define INF (99999)

#define DISABLE_TIME 1

int searchGetTime(searchStat* stat) {
	return (int)(time(NULL) - stat->startSearchTIme);
}

void searchCheckTime(searchStat* stat) {
#if DISABLE_TIME == 1
	if (searchGetTime(stat) > stat->maxSearchTime)
		gStopSearch = 1;
#endif
}

void UciInfo(int depth, int node, int score, searchStat* stat) {

	printf("info depth %i ", depth);
	printf("nodes %i ", node);
	printf("score cp %i ", score);
	printf("time 15 ");
	printf("pv ");
	/*for (int ii = 0; ii < stat->pv._nbrMove; ii++) {
		movePrintShort(&stat->pv._sMoveList[ii]);
	}*/

	printf("\n");

}

smove searchStart(sboard* pBoard, int wtime, int btime, int moveToGo, searchStat* stat) {

	smove bestMove;

	if (pBoard->_ActivePlayer == WHITE) {
		stat->maxSearchTime = wtime;
	}
	else {
		stat->maxSearchTime = btime;
	}

	if (moveToGo > 0) {
		stat->maxSearchTime = stat->maxSearchTime / moveToGo;
		stat->maxSearchTime = stat->maxSearchTime / 1000; //milli seconde to seconde
	}
	else {
		stat->maxSearchTime = 1;
	}

	gStopSearch = 0;
	stat->nbrNode = 0;
	stat->nbrCut = 0;
	stat->nbrZob = 0;

	time(&stat->startSearchTIme);

	gNodeCptCheckTime = 0;

	moveOrderClearKiller();
	 
	for (int depth = 2; depth < 20; depth++) {
		stat->maxDepth = depth;
		stat->boardEval= negamax(pBoard, stat->maxDepth, pBoard->_ActivePlayer, stat);
		//negamaxTT(pBoard, stat->maxDepth, -INF, INF, pBoard->_ActivePlayer, stat);
		UciInfo(depth, stat->nbrNode, stat->boardEval, stat);
		if (gStopSearch)
			return bestMove;
		moveCpy(&bestMove, &pBoard->_bestMove);
		if (searchGetTime(stat) > stat->maxSearchTime / 2)
			return bestMove;
		if (stat->boardEval == INF || stat->boardEval == -INF)
			return bestMove;
	}
	return pBoard->_bestMove;
}

int negamax(sboard* pNode, int depth,Color color, searchStat* stat) {
	smoveList mliste;
	sboard child;

	if (gStopSearch)
		return 0;

	boardGenerateAllLegalMoves(pNode, &mliste);
	// Check for checkmate and stalemate
	if (mliste._nbrMove == 0) {
		if (colorIsInCheck(pNode, pNode->_ActivePlayer))
		{
			return -INF;
		}
		return 0;// mate
	}

	if (depth == 0) { // or node is a terminal node then
		stat->nbrNode++;

		gNodeCptCheckTime++;
		if (gNodeCptCheckTime > 100000) {
			searchCheckTime(stat);
			gNodeCptCheckTime = 0;
		}
		int res = evaluate(pNode);
		return (pNode->_ActivePlayer == WHITE) ? res : -res;
		
	}


	moveOrder(&mliste, depth, stat);

	int value = -INF;
	int score;
	for (int ii = 0; ii < mliste._nbrMove; ii++) {
	 
		/* function de debug
		for (int jj = 0; jj < (stat->maxDepth-depth); jj++)
		{
			printf(" ");
		}
		movePrintShort(&mliste._sMoveList[ii]);
		printf(" \n");
		*/
		 
		boardCpy(&child, pNode);
		doMove(&child, &mliste._sMoveList[ii]);
		score =  -negamax(&child, depth - 1, !color, stat);
		 
		if (score >= value)
		{
			moveCpy(&pNode->_bestMove, &mliste._sMoveList[ii]);			
			value = score;			
		 }
	}
	return value;
}
 