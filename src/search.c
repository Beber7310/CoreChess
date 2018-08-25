/*
 * search.c
 *
 *  Created on: 21 août 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "search.h"
#include "evaluate.h"
#include <sys/time.h>
#include <time.h>

time_t startSearchTIme;

int stopSearch;
int maxSearchTime;
int nbrNode;
int nbrNodeCheck;

#define INF (99999)

int searchGetTime() {
	return time(NULL) - startSearchTIme;
}

void searchCheckTime() {
	if (searchGetTime() > maxSearchTime)
		stopSearch = 1;

}

int max(int a, int b) {
	if (a > b)
		return a;
	else
		return b;
}


void logUci(int depth,int node,int score)
{

	  printf("info depth %i ",depth);
	  printf("seldepth %i ",depth);
	  printf("nodes %i ",node);
	  printf("score cp %i ",score);
	  printf("time %i pv a7a5\n",searchGetTime());


}
smove searchStart(sboard * pBoard, int wtime, int btime, int moveToGo) {

	smove bestMove;

	time(&startSearchTIme);

	if (pBoard->_ActivePlayer == WHITE) {
		maxSearchTime = wtime;
	} else {
		maxSearchTime = btime;
	}

	if (moveToGo > 0) {
		maxSearchTime = maxSearchTime / moveToGo;
		maxSearchTime =maxSearchTime/1000; 			//milli seconde to seconde
	} else {
		maxSearchTime = 5;
	}
	stopSearch = 0;
	nbrNode = 0;
	nbrNodeCheck = 0;

	for (int depth = 1; depth < 20; depth++) {
		int res=negamax(pBoard, depth, -INF, INF, pBoard->_ActivePlayer);
		logUci(depth,nbrNode,res);
		if (stopSearch)
			return bestMove;
		moveCpy(&bestMove, &pBoard->_bestMove);
	}
	return pBoard->_bestMove;
}

int negamax(sboard * pNode, int depth, int alpha, int beta, Color color) {
	smoveList mliste;
	sboard child;

	if (stopSearch)
		return 0;

	if (depth == 0) { // or node is a terminal node then
		nbrNode++;
		nbrNodeCheck++;

		if (nbrNodeCheck > 100000) {
			searchCheckTime();

			nbrNodeCheck=0;
		}
		int res = evaluate(pNode);

		if (color == WHITE)
			return res;
		else
			return -res;
	}

	boardGenerateAllLegalMoves(pNode, &mliste);

	int value = -INF;
	for (int ii = 0; ii < mliste._nbrMove; ii++) {
		boardCpy(&child, pNode);

		doMove(&child, &mliste._sMoveList[ii]);
		value = max(value, -negamax(&child, depth - 1, -beta, -alpha, !color));

		if (stopSearch)
			return 0;

		if (value > alpha)
			moveCpy(&pNode->_bestMove, &mliste._sMoveList[ii]);

		alpha = max(alpha, value);
		if (alpha >= beta) {
			return value;	//break (* cut-off *)
		}
	}
	return value;
}
