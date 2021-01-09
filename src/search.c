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
#include "book.h"

int gStopSearch;			// used to stop digging when time is over
int gNodeCptCheckTime;		//Used as a counter to choose when to print some info
extern int uciOptionQuiesence;

int searchGetTime(searchStat* stat) {
	return (int)(time(NULL) - stat->startSearchTIme);
}

void searchCheckTime(searchStat* stat) {
	if (searchGetTime(stat) >= stat->maxSearchTime)
		gStopSearch = 1;
}

void UciInfo(int depth, int node, int score, searchStat* stat) {
	char str[128];

	/*
	printf("info depth %i ", depth);
	printf("nodes %i ", node);
	printf("score %i ", score);
	printf("\n");
	*/

	snprintf(str, sizeof(str) - 1, "info depth %i nodes %i score %i\n", depth, node, score);
	printTcp(str);
}


int negamaxTT(sboard* pBoard, int depth, int alpha, int beta, searchStat* statistics, search_state state) {
	smoveList mliste;
	sboard child;
	ttEntry* tt;

	int alphaOrig = alpha;

	if (gStopSearch)
		return 0;

	if (depth != 0 && state != SEARCH_FIRST)
	{
		tt = ttGet(pBoard->_zobKey);
		if (tt) {
			if (tt->flag != EMPTY) {
				if (tt->depth >= (depth)) {
					statistics->nbrZob++;
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
						printf("Must not happen!(negamaxTT)");
						break;
					}
					if (alpha >= beta)
					{
						return tt->value;
					}
				}
			}
		}
	}

	if (state == SEARCH_FIRST)
	{
		state = SEARCH_ALPHA;
	}

	boardGenerateAllLegalMoves(pBoard, &mliste);
	// Check for checkmate and stalemate
	if (mliste._nbrMove == 0) {
		statistics->nbrNode++;
		if (colorIsInCheck(pBoard, pBoard->_ActivePlayer)) {
				return -INF;			
		}
		return 0; // mate
	}

	if (depth == 0) { // or node is a terminal node then
		statistics->nbrNode++;
		gNodeCptCheckTime++;
		if (gNodeCptCheckTime > 100000) {
			searchCheckTime(statistics);
			gNodeCptCheckTime = 0;
		}

		if (!uciOptionQuiesence)
		{
			int res = evaluate(pBoard);
			if (pBoard->_ActivePlayer == WHITE)
				return res;
			else
				return -res;
		}
		else
		{
			if (state == SEARCH_ALPHA)
			{
				depth = 5;
				statistics->nbrQuies++;
				state = SEARCH_QUIESSENCE;
				return Quiessence(pBoard, depth, alpha, beta, statistics, state);
			}
			else if (state == SEARCH_QUIESSENCE)
			{
				int res = evaluate(pBoard);
				if (pBoard->_ActivePlayer == WHITE)
					return res;
				else
					return -res;
			}
		}


	}

	moveOrder(&mliste, depth, state == SEARCH_QUIESSENCE, statistics); // devrait rajouter le filtre de move qui capture rien

	int value = -INF;
	for (int ii = 0; ii < mliste._nbrMove; ii++) {

		boardCpy(&child, pBoard);
		doMove(&child, &mliste._sMoveList[ii]);

		value = max(value, -negamaxTT(&child, depth - 1, -beta, -alpha, statistics, state));

		if (value > alpha) {
			moveCpy(&pBoard->_bestMove, &mliste._sMoveList[ii]);
			alpha = value;
		}

		if (alpha >= beta) {
			statistics->nbrCut++;
			mliste._sMoveList[ii]._value = alpha;
			moveOrderAddKiller(&mliste._sMoveList[ii], depth);
			break;  //break (* cut-off *)
		}
	}
	 

	// Save the result in the hash db
	if (state != SEARCH_QUIESSENCE)
	{
		uint8_t flag = 0;
		if (value <= alphaOrig) {
			flag = UPPERBOUND;
		}
		else if (value >= beta) {
			flag = LOWERBOUND;
		}
		else {
			flag = EXACT;
		}
		ttSet(pBoard, value, depth, flag);
	}

	// alpha is equal to -INF if this pos is always leading to mat. so we peek the first move to avoid leting best move empty
	if ((alpha == -INF) && (mliste._nbrMove > 0))
	{
		moveCpy(&pBoard->_bestMove, &mliste._sMoveList[0]);
	}


	return value;
}


/// <summary>
/// Entry point of the search algo. 
/// If moveTo Go is > 0 we use wtime and btime otherwise we use mtime
/// </summary>
/// <param name="pBoard"></param>
/// <param name="wtime"></param>
/// <param name="btime"></param>
/// <param name="moveToGo"></param>
/// <param name="stat"></param>
/// <returns></returns>
smove searchStart(sboard* pBoard, int wtime, int btime, int mtime, int moveToGo, searchStat* stat) {

	smove bestMove;


	bestMove = getBookMove(pBoard, BOOK_NARROW);
	if (bestMove._move != 0) {
		return bestMove;
	}

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
		stat->maxSearchTime = mtime / 1000;
	}

	gStopSearch = 0;
	stat->nbrNode = 0;
	stat->nbrCut = 0;
	stat->nbrZob = 0;
	stat->nbrQuies = 0;

	time(&stat->startSearchTIme);

	gNodeCptCheckTime = 0;

	moveOrderClearKiller();

	for (int depth = 3; depth < 20; depth++) {
		stat->maxDepth = depth;
		stat->boardEval = negamaxTT(pBoard, depth, -INF, INF, stat, SEARCH_FIRST);
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



