/*
 * search.c
 *
 *  Created on: 21 août 2018
 *      Author: Bertrand
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>     /* assert */
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

#define DISABLE_TIME 0

int searchGetTime(searchStat* stat) {
	return (int) (time(NULL) - stat->startSearchTIme);
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
	printf("score %i ", score);

	printf("\n");

}

smove searchStart(sboard* pBoard, int wtime, int btime, int moveToGo, searchStat* stat) {

	smove bestMove;

	if (pBoard->_ActivePlayer == WHITE) {
		stat->maxSearchTime = wtime;
	} else {
		stat->maxSearchTime = btime;
	}

	if (moveToGo > 0) {
		stat->maxSearchTime = stat->maxSearchTime / moveToGo;
		stat->maxSearchTime = stat->maxSearchTime / 1000; //milli seconde to seconde
	} else {
		stat->maxSearchTime = 5;
	}

	gStopSearch = 0;
	stat->nbrNode = 0;
	stat->nbrCut = 0;
	stat->nbrZob = 0;

	time(&stat->startSearchTIme);

	gNodeCptCheckTime = 0;

	moveOrderClearKiller();

	for (int depth = 3; depth < 20; depth++) {
		stat->maxDepth = depth;
		stat->boardEval = negamaxTT(pBoard, depth, -INF, INF, pBoard->_ActivePlayer, stat);
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

smove algoCheck(sboard* pBoard, AlgoFunc algo1, AlgoFunc algo2, searchStat* stat) {

	smove bestMove;

	searchStat statAlgo1; // Negamax
	searchStat statAlgo2; // AlphaBeta

	sboard boardAlgo1;
	sboard boardAlgo2;

	memcpy(&boardAlgo1, pBoard, sizeof(sboard));
	memcpy(&boardAlgo2, pBoard, sizeof(sboard));

	stat->maxSearchTime = 60;

	gStopSearch = 0;
	stat->nbrNode = 0;
	stat->nbrCut = 0;
	stat->nbrZob = 0;

	memcpy(&statAlgo1, stat, sizeof(searchStat));
	memcpy(&statAlgo2, stat, sizeof(searchStat));

	time(&statAlgo1.startSearchTIme);

	gNodeCptCheckTime = 0;

	moveOrderClearKiller();

	for (int depth = 2; depth < 20; depth++) {
		int err = 0;

		statAlgo1.maxDepth = depth;
		statAlgo2.maxDepth = depth;

		statAlgo1.boardEval = algo1(&boardAlgo1, statAlgo1.maxDepth, pBoard->_ActivePlayer, -INF, INF, &statAlgo1);
		statAlgo2.boardEval = algo2(&boardAlgo2, statAlgo2.maxDepth, pBoard->_ActivePlayer, -INF, INF, &statAlgo2);

		if (statAlgo1.boardEval == statAlgo2.boardEval) {
			stat->boardEval = statAlgo1.boardEval;
			if ((statAlgo1.boardEval == INF) || (statAlgo1.boardEval == -INF)) {
				if (boardAlgo1._bestMove._move != boardAlgo2._bestMove._move) {
					printTcp("Error alphabeta and Negamax found different bestmove \n");
					movePrint(&boardAlgo1._bestMove);
					movePrint(&boardAlgo2._bestMove);
					err++;
				}
			}
		} else {
			if ((abs(statAlgo1.boardEval) != INF) && (abs(statAlgo2.boardEval) != INF)) {
				printTcp("Error alphabeta and Negamax found different eval \n");
				err++;
			}
		}

		memcpy(pBoard, &boardAlgo2, sizeof(sboard));
		memcpy(stat, &statAlgo2, sizeof(searchStat));

		printTcp("NM :");
		movePrintShort(&boardAlgo1._bestMove);
		UciInfo(depth, statAlgo1.nbrNode, statAlgo1.boardEval, &statAlgo1);

		printTcp("AB :");
		movePrintShort(&boardAlgo2._bestMove);
		UciInfo(depth, statAlgo2.nbrNode, statAlgo2.boardEval, &statAlgo2);

		if (err) {

			stat->boardEval = 0;
			return pBoard->_bestMove;
		}

		if (gStopSearch) {
			printf("searchStart stop by timeout \n");
			return bestMove;
		}

		moveCpy(&bestMove, &boardAlgo2._bestMove);
		if (searchGetTime(&statAlgo1) > statAlgo1.maxSearchTime / 2)
			return bestMove;
		if (statAlgo1.boardEval == INF || statAlgo1.boardEval == -INF)
			return bestMove;
	}

	return pBoard->_bestMove;
}

int negamax(sboard* pNode, int depth, Color color, int alpha, int beta, searchStat* stat) {
	smoveList mliste;
	sboard child;

	if (gStopSearch)
		return 0;

	boardGenerateAllLegalMoves(pNode, &mliste);
	// Check for checkmate and stalemate
	if (mliste._nbrMove == 0) {
		stat->nbrNode++;
		if (colorIsInCheck(pNode, pNode->_ActivePlayer)) {
			return -INF;
		}
		return 0; // mate
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
		 /*	if ((stat->maxDepth - depth) == 0)
			moveBreakPoint(&mliste._sMoveList[ii], "d4b5");
		if ((stat->maxDepth - depth) == 1)
			moveBreakPoint(&mliste._sMoveList[ii], "c3c4");
		if ((stat->maxDepth - depth) == 2)
			moveBreakPoint(&mliste._sMoveList[ii], "b5d6");*/

		boardCpy(&child, pNode);
		doMove(&child, &mliste._sMoveList[ii]);
		score = -negamax(&child, depth - 1, !color, alpha, beta, stat);

		if (score >= value) {
			moveCpy(&pNode->_bestMove, &mliste._sMoveList[ii]);
			value = score;
		}
	}
	return value;
}

int alphaBeta(sboard* pNode, int depth, Color color, int alpha, int beta, searchStat* stat) {
	smoveList mliste;
	sboard child;

	if (gStopSearch)
		return 0;

	boardGenerateAllLegalMoves(pNode, &mliste);
	// Check for checkmate and stalemate
	if (mliste._nbrMove == 0) {
		stat->nbrNode++;
		if (colorIsInCheck(pNode, pNode->_ActivePlayer)) {
			return -INF;
		}
		return 0; // mate
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

	int value;
	int best = -INF;

	for (int ii = 0; ii < mliste._nbrMove; ii++) {

		/*for (int jj = 0; jj < (stat->maxDepth - depth); jj++)
		 {
		 printf(" ");
		 }
		 movePrintShort(&mliste._sMoveList[ii]);
		 printf(" \n");
		 */
		boardCpy(&child, pNode);
		doMove(&child, &mliste._sMoveList[ii]);
		value = -alphaBeta(&child, depth - 1, !color, -beta, -alpha, stat);
		mliste._sMoveList[ii]._value = value;
		if (value >= best) {
			moveCpy(&pNode->_bestMove, &mliste._sMoveList[ii]);
			best = value;
			if (best > alpha) {
				alpha = best;
				if (alpha > beta) {
					moveOrderAddKiller(&mliste._sMoveList[ii], depth);
					stat->nbrCut++;
					return best;
				}
			}
		}
	}

	return best;
}

int negascout(sboard* pNode, int depth, Color color, int alpha, int beta, searchStat* stat) {
	smoveList mliste;
	sboard child;

	if (gStopSearch)
		return 0;

	boardGenerateAllLegalMoves(pNode, &mliste);
	// Check for checkmate and stalemate
	if (mliste._nbrMove == 0) {
		stat->nbrNode++;
		if (colorIsInCheck(pNode, pNode->_ActivePlayer)) {
			return -INF;
		}
		return 0; // mate
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

	int bestscore = 0;
	int score;

	assert(mliste._nbrMove > 0);

	for (int ii = 0; ii < mliste._nbrMove; ii++) {

		boardCpy(&child, pNode);
		doMove(&child, &mliste._sMoveList[ii]);
		if (ii == 0) {
			bestscore = -negascout(&child, depth - 1, !color, -beta, -alpha, stat);
			moveCpy(&pNode->_bestMove, &mliste._sMoveList[ii]);
			if (bestscore > alpha) {
				if (bestscore > beta) {
					stat->nbrCut++;
					return bestscore;
				}
				alpha = bestscore;
			}
		} else {
			score = -negascout(&child, depth - 1, !color, -alpha - 1, -alpha, stat);
			if ((score > alpha) && (score < beta)) {
				score = -negascout(&child, depth - 1, !color, -beta, -alpha, stat);
				if (score > alpha)
					alpha = score;
			}

			if (score > bestscore) {
				moveCpy(&pNode->_bestMove, &mliste._sMoveList[ii]);
				if (score > beta) {
					mliste._sMoveList[ii]._value = score;
					moveOrderAddKiller(&mliste._sMoveList[ii], depth);
					stat->nbrCut++;
					return score;
				}

				bestscore = score;
			}
		}
	}

	return bestscore;
}
