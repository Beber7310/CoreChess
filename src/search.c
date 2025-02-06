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
#include "moveOrder.h"
#include "transposition.h"
#include "book.h"
#include "tcpserver.h"
#include "quiescence.h"

int gStopSearch;			// used to stop digging when time is over
int gNodeCptCheckTime;		//Used as a counter to choose when to print some info
extern int uciOptionQuiesence;

int searchGetTime(negaMaxConf* stat) {	
	return (int)(time(NULL) - stat->startSearchTIme);
}

void searchCheckTime(negaMaxConf* stat) {
	int time= searchGetTime(stat);
	if (time >= stat->maxSearchTime)
	{
		printf("searchCheckTime Timeout!!! %i\n", time);
		gStopSearch = 1;
	}
}

void UciInfo(int depth, int node, int score, negaMaxConf* stat) {
	char str[128];


	snprintf(str, sizeof(str) - 1, "info depth %i nodes %9i score cp %5i\n", depth, node, score);
	printTcp(str);
}

int negamaxTT(sboard* pBoard, int depth, int alpha, int beta, negaMaxConf* statistics, search_state state, smoveList* pastMoves, smoveList* pvMoves) {
	smoveList mliste;
	sboard child;
	ttEntry* tt;

	int alphaOrig = alpha;

//	printf("depth: %i\n", depth);
	if (gStopSearch)
		return 0;

	if (depth != statistics->maxDepth && state != SEARCH_FIRST)
	{
		tt = ttGet(pBoard->_zobKey);
		if (tt) {
			if (tt->flag != EMPTY) {
				if (tt->depth >= (statistics->maxDepth-depth)) {
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

	if (depth == statistics->maxDepth) { // or node is a terminal node then
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

	moveOrder(&mliste, depth, state == SEARCH_QUIESSENCE, statistics);

	int value = -INF;
	for (int ii = 0; ii < mliste._nbrMove; ii++) {
		int drawCnt = 0;

		boardCpy(&child, pBoard);
		doMove(&child, &mliste._sMoveList[ii]);

		if (pastMoves)
		{
			moveCpy(&pastMoves->_sMoveList[depth], &mliste._sMoveList[ii]);
			// We look for draw
			for (drawCnt = 0; drawCnt < 6; drawCnt++)
			{
				if (pastMoves->_sMoveList[depth + drawCnt]._move != pastMoves->_sMoveList[depth + 2 + drawCnt]._move)
				{
					break;
				}
			}
		}
		if (drawCnt == 6) { // it's a draw!
			value = 0;
		}else {
			value = max(value, -negamaxTT(&child, depth + 1, -beta, -alpha, statistics, state, pastMoves, pvMoves));			
		}
		if (value > alpha) {
			moveCpy(&pBoard->_bestMove, &mliste._sMoveList[ii]);
			alpha = value;
		}

		if (alpha >= beta) {
			statistics->nbrCut++;
			mliste._sMoveList[ii]._value = alpha;
			moveOrderAddKiller(&mliste._sMoveList[ii], depth);//TBC
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
		ttSet(pBoard, value, statistics->maxDepth - depth, flag);
	}

	// alpha is equal to -INF if this pos is always leading to mat. so we peek the first move to avoid leting best move empty
	if ((alpha == -INF) && (mliste._nbrMove > 0))
	{
		moveCpy(&pBoard->_bestMove, &mliste._sMoveList[0]);
	}

	return value;
}


int findPV(sboard* pBoard, int depth, smoveList * pvMove)
{
	sboard sBoard, eBoard;
	negaMaxConf stat; 
	boardCpy(&sBoard, pBoard);
	char strMove[10];

	moveListInit(pvMove);

	stat.maxSearchTime = 5;
	time(&stat.startSearchTIme);

	for (int ii = 0; ii < depth; ii++)
	{
		negamaxTT(&sBoard, depth - ii, -INF, INF, &stat, SEARCH_FIRST, 0,0);
		boardCpy(&eBoard, &sBoard);
		doMove(&eBoard, &eBoard._bestMove);
		boardCpy(&sBoard, &eBoard);
		
		moveCpy(&pvMove->_sMoveList[pvMove->_nbrMove],&eBoard._bestMove);
		pvMove->_nbrMove++;

		movePrintShort(&eBoard._bestMove, strMove);
		printTcp(strMove);

	}
	printTcp("\n");
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
smove searchStart(sboard* pBoard, int wtime, int btime, int winc, int binc, int moveToGo, negaMaxConf* stat, smoveList* pastMoves) {
	smoveList pvMove;
	smove bestMove;
	int incr = 0;
	int player_time = 0;
	
	bestMove = getBookMove(pBoard, BOOK_NARROW);
	if (bestMove._move != 0) {
		return bestMove;
	}

	if (pBoard->_ActivePlayer == WHITE) {
		player_time = wtime + (wtime - btime) / 4;
		incr = winc;
	}
	else {
		player_time = btime + (btime - wtime) / 4;
		incr = binc;
	}
 
	if (player_time < 100)
		player_time = 100;

	if (player_time > 100000)
		player_time = 100000;

 	if (moveToGo < 1)
		moveToGo = 10;

	player_time = player_time / moveToGo;
 
	stat->maxSearchTime = (player_time + incr) / 1000;

 
	gStopSearch = 0;
	stat->nbrNode = 0;
	stat->nbrCut = 0;
	stat->nbrZob = 0;
	stat->nbrQuies = 0;

	time(&stat->startSearchTIme);

	gNodeCptCheckTime = 0;

	moveOrderClearKiller();
	 

	for (int depth = 3; depth < MAX_SEARCH_DEPTH; depth++) {
		smoveList pastMovesDraw;
		moveListInit(&pastMovesDraw);
		stat->maxDepth = depth;

		if (pastMoves)
			for (int ii = 0; ii < 6; ii++)
			{
				moveCpy(&pastMovesDraw._sMoveList[depth + ii], &pastMoves[depth + ii]);
			}

		if (depth < 5) {
			stat->boardEval = negamaxTT(pBoard, 1, -INF, INF, stat, SEARCH_FIRST, &pastMovesDraw, &pvMove);
		}
		else {
			int alpha = stat->boardEval - 100;
			int beta = stat->boardEval + 100;
			int value = negamaxTT(pBoard, 1, alpha, beta, stat, SEARCH_FIRST, &pastMovesDraw, &pvMove);
			if (value <= alpha || value >= beta)
			{
				value = negamaxTT(pBoard, 1, -INF, INF, stat, SEARCH_FIRST, &pastMovesDraw, &pvMove);
			}
			stat->boardEval = value;
		}

		UciInfo(depth, stat->nbrNode, stat->boardEval, stat);

		//findPV(pBoard, depth,&pvMove);

		if (gStopSearch)
		{
			//printf("Timeout!!!\n");
			return bestMove;
		}
		moveCpy(&bestMove, &pBoard->_bestMove);
		if (searchGetTime(stat) > stat->maxSearchTime / 2)
			return bestMove;
		if (stat->boardEval == INF || stat->boardEval == -INF)
			return bestMove;
	}
	return pBoard->_bestMove;
}



