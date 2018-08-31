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

#define DISABLE_TIME 0

int searchGetTime(searchStat* stat) {
	return time(NULL) - stat->startSearchTIme;
}

void searchCheckTime(searchStat* stat) {
#if DISABLE_TIME == 1
	if (searchGetTime() > stat->maxSearchTime)
	stopSearch = 1;
#endif
}

void UciInfo(int depth, int node, int score, searchStat* stat) {
	printf("info depth %i ", depth);
	printf("nodes %i ", node);
	printf("score cp %i ", score);
	printf("time 15 ");
	printf("pv ");
	for (int ii = 0; ii < stat->pv._nbrMove; ii++) {
		movePrintShort(&stat->pv._sMoveList[ii]);
	}

	printf("\n");

}

smove searchStart(sboard * pBoard, int wtime, int btime, int moveToGo, searchStat* stat) {

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

	time(&stat->startSearchTIme);

	gNodeCptCheckTime = 0;

	for (int depth = 1; depth < 20; depth++) {
		stat->maxDepth = depth;
		//negamax(pBoard, 0, -INF, INF, pBoard->_ActivePlayer, stat);
		negamaxTT(pBoard, 0, -INF, INF, pBoard->_ActivePlayer, stat);
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

int negamax(sboard * pNode, int depth, int alpha, int beta, Color color, searchStat* stat) {
	smoveList mliste;
	sboard child;

	if (gStopSearch)
		return 0;

	if (depth == stat->maxDepth) { // or node is a terminal node then
		stat->nbrNode++;
		gNodeCptCheckTime++;

		if (gNodeCptCheckTime > 100000) {
			searchCheckTime(stat);
			gNodeCptCheckTime = 0;
		}
		int res = evaluate(pNode);

		if (color == WHITE)
			return res;
		else
			return -res;
	}

	boardGenerateAllLegalMoves(pNode, &mliste);
	// Check for checkmate and stalemate
	if (mliste._nbrMove == 0) {
		return colorIsInCheck(pNode, pNode->_ActivePlayer) ? -INF : 0;;
	}

	moveOrder(&mliste, stat);

	int value = -INF;
	for (int ii = 0; ii < mliste._nbrMove; ii++) {
		boardCpy(&child, pNode);

		doMove(&child, &mliste._sMoveList[ii]);
		value = max(value, -negamax(&child, depth + 1, -beta, -alpha, !color, stat));

		if (gStopSearch)
			return 0;

		if (value > alpha) {
			moveCpy(&pNode->_bestMove, &mliste._sMoveList[ii]);
			moveCpy(&stat->pv._sMoveList[depth], &mliste._sMoveList[ii]);
			stat->pv._nbrMove = stat->maxDepth;
			stat->boardEval = value;
			alpha = value;
		}

		//alpha = max(alpha, value);
		if (alpha >= beta) {
			stat->nbrCut++;
			moveOrderAddKiller(&mliste._sMoveList[ii]);
			return value;	//break (* cut-off *)
		}
	}
	return value;
}

int negamaxTT(sboard * pNode, int depth, int alpha, int beta, Color color, searchStat* stat) {
	smoveList mliste;
	ttEntry* tt;
	sboard child;

	int alphaOrig = alpha;

	if (gStopSearch)
		return 0;

	//ttEntry := transpositionTableLookup(node)
	tt = ttGet(pNode->_zobKey);
	if (depth != 0)
		if (tt) {
			if (tt->flag != EMPTY) {
				if (tt->depth >= (stat->maxDepth - depth)) {
					switch (tt->flag) {
					case EXACT:
						return tt->value;
						break;
					case UPPERBOUND:
						beta = min(beta, tt->value);
						break;
					case LOWERBOUND:
						alpha = max(alpha, tt->value);
						break;
					case EMPTY:
						printf("Mustnot happen!(negamaxTT)");
						break;
					}
				}
				if (alpha >= beta)
					return tt->value;
			}
		}

	if (depth == stat->maxDepth) { // or node is a terminal node then
		stat->nbrNode++;
		gNodeCptCheckTime++;

		if (gNodeCptCheckTime > 100000) {
			searchCheckTime(stat);
			gNodeCptCheckTime = 0;
		}
		int res = evaluate(pNode);

		if (color == WHITE)
			return res;
		else
			return -res;
	}

	boardGenerateAllLegalMoves(pNode, &mliste);
// Check for checkmate and stalemate
	if (mliste._nbrMove == 0) {
		return colorIsInCheck(pNode, pNode->_ActivePlayer) ? -INF : 0;;
	}

	moveOrder(&mliste, stat);

	int value = -INF;
	for (int ii = 0; ii < mliste._nbrMove; ii++) {
		boardCpy(&child, pNode);

		doMove(&child, &mliste._sMoveList[ii]);
		value = max(value, -negamaxTT(&child, depth + 1, -beta, -alpha, !color, stat));

		if (gStopSearch)
			return 0;

		if (value > alpha) {
			moveCpy(&pNode->_bestMove, &mliste._sMoveList[ii]);
			moveCpy(&stat->pv._sMoveList[depth], &mliste._sMoveList[ii]);
			stat->pv._nbrMove = stat->maxDepth;
			stat->boardEval = value;
			alpha = value;
		}

		//alpha = max(alpha, value);
		if (alpha >= beta) {
			stat->nbrCut++;
			break;
			//return value;	//break (* cut-off *)
		}
	}

	uint8_t flag = 0;
	;
	if (value <= alphaOrig)
		flag = UPPERBOUND;
	else if (value <= beta)
		flag = LOWERBOUND;
	else
		flag = EXACT;

	ttSet(pNode, value, stat->maxDepth - depth, flag);

	return value;
}

