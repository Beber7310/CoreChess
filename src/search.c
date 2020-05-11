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

	searchStat statNM; // Negamax
	searchStat statAB; // AlphaBeta

	sboard boardNM;
	sboard boardAB;

	memcpy(&boardNM, pBoard, sizeof(sboard));
	memcpy(&boardAB, pBoard, sizeof(sboard));

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
		stat->maxSearchTime = 60;
	}

	gStopSearch = 0;
	stat->nbrNode = 0;
	stat->nbrCut = 0;
	stat->nbrZob = 0;

	memcpy(&statAB, stat, sizeof(searchStat));
	memcpy(&statNM, stat, sizeof(searchStat));

	time(&statNM.startSearchTIme);

	gNodeCptCheckTime = 0;

	moveOrderClearKiller();

	for (int depth = 2; depth < 20; depth++) {
		int err = 0;

		statNM.maxDepth = depth;
		statAB.maxDepth = depth;



		statNM.boardEval = negamax(&boardNM, statNM.maxDepth, pBoard->_ActivePlayer, -INF, INF, &statNM);
		statAB.boardEval = alphaBeta(&boardAB, statAB.maxDepth, pBoard->_ActivePlayer, -INF, INF, &statAB);

		/*
		moveOrderClearKiller();
		statNM.boardEval = alphaBeta(&boardNM, statNM.maxDepth, pBoard->_ActivePlayer, -INF, INF, &statNM);
		moveOrderClearKiller();
		statAB.boardEval = negascout(&boardAB, statNM.maxDepth, pBoard->_ActivePlayer, -INF, INF, &statAB);
		*/

		if (statNM.boardEval == statAB.boardEval) {
			stat->boardEval = statNM.boardEval;
			if ((statNM.boardEval == INF) || (statNM.boardEval == -INF))
			{
				if (boardNM._bestMove._move != boardAB._bestMove._move) {
					printTcp("Error alphabeta and Negamax found different bestmove \n");
					movePrint(&boardNM._bestMove);
					movePrint(&boardAB._bestMove);
					err++;
				}
			}
		}
		else {
			if ((statNM.boardEval != INF) && (statNM.boardEval != -INF))
			{
				printTcp("Error alphabeta and Negamax found different eval \n");
				err++;
			}
		}

		memcpy(pBoard, &boardAB, sizeof(sboard));
		memcpy(stat, &statAB, sizeof(searchStat));

		printTcp("NM :");
		movePrintShort(&boardNM._bestMove);
		UciInfo(depth, statNM.nbrNode, statNM.boardEval, &statNM);

		printTcp("AB :");
		movePrintShort(&boardAB._bestMove);
		UciInfo(depth, statAB.nbrNode, statAB.boardEval, &statAB);

		if (err) {

			stat->boardEval = 0;
			return pBoard->_bestMove;
		}

		if (gStopSearch)
		{
			printf("searchStart stop by timeout \n");
			return bestMove;
		}

		moveCpy(&bestMove, &boardAB._bestMove);
		if (searchGetTime(&statNM) > statNM.maxSearchTime / 2)
			return bestMove;
		if (statNM.boardEval == INF || statNM.boardEval == -INF)
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

//	moveOrder(&mliste, depth, stat);

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

//	moveOrder(&mliste, depth, stat);

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
		if (ii == 0)
		{
			bestscore = -negascout(&child, depth - 1, !color, -beta, -alpha, stat);
			moveCpy(&pNode->_bestMove, &mliste._sMoveList[ii]);
			if (bestscore > alpha)
			{
				if (bestscore > beta)
				{
					stat->nbrCut++;
					return bestscore;
				}
				alpha = bestscore;
			}
		}
		else
		{
			score = -negascout(&child, depth - 1, !color, -alpha - 1, -alpha, stat);
			if ((score > alpha) && (score < beta))
			{
				score = -negascout(&child, depth - 1, !color, -beta, -alpha, stat);
				if (score > alpha)
					alpha = score;
			}

			if (score > bestscore)
			{
				moveCpy(&pNode->_bestMove, &mliste._sMoveList[ii]);
				if (score > beta)
				{
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
