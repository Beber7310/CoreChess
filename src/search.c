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
#include "polyglot.h"

#ifdef _MSC_VER
#include <sys\timeb.h> 
#else
#include <sys/timeb.h> 
#endif



int gStopSearch;			// used to stop digging when time is over
int gNodeCptCheckTime;		//Used as a counter to choose when to print some info
extern int uciOptionQuiesence;




int searchGetTime(negaMaxConf * stat) {
	struct timeb tp;
	ftime(&tp);
	int delta = 1000 * (tp.time - stat->startSearchTIme.time);
	delta += tp.millitm - stat->startSearchTIme.millitm;

	return delta;
}

void searchCheckTime(negaMaxConf * stat) {
	char str[128];
	int time = searchGetTime(stat);
	//snprintf(str, 128, "current time: %i\n", time);
	//printTcp(str);
	if (time >= stat->maxSearchTime)
	{
		printf("searchCheckTime Timeout!!! %i\n", time);
		gStopSearch = 1;
	}
}

void UciInfo(int depth, int node, int score, negaMaxConf * stat) {
	char str[128];


	snprintf(str, sizeof(str) - 1, "info depth %i nodes %9i score cp %5i\n", depth, node, score);
	printTcp(str);
}

int negamaxTT(sboard * pBoard, int depth, int alpha, int beta, negaMaxConf * statistics, search_state state, smoveList * pastMoves, smoveList * pvMoves) {
	smoveList mliste;
	sboard child;
	ttEntry* tt;

	int alphaOrig = alpha;
	// printf("1\n");//DEBUG
	if (gStopSearch)
		return 0;

	if (state != SEARCH_FIRST)
	{
		tt = ttGet(pBoard->_zobKey);
		if (tt) {
			if (tt->flag != EMPTY) {
				if (tt->depth >= (statistics->maxDepth - depth)) {
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
	// printf("2\n");//DEBUG
	if (state == SEARCH_FIRST)
	{
		state = SEARCH_ALPHA;
	}

	if (depth == statistics->maxDepth) {	// node is a terminal node then
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

	// printf("3\n");//DEBUG

	boardGenerateAllLegalMoves(pBoard, &mliste);

	// printf("4\n");//DEBUG

	if (mliste._nbrMove == 0) {				// Check for checkmate and stalemate
		statistics->nbrNode++;
		if (colorIsInCheck(pBoard, pBoard->_ActivePlayer)) {
			return -INF;
		}
		return 0; // mate
	}

	// printf("5\n");//DEBUG

	moveOrder(&mliste, depth, state == SEARCH_QUIESSENCE, pvMoves, statistics);

	// printf("6\n");//DEBUG

	int value = -INF;
	for (int ii = 0; ii < mliste._nbrMove; ii++) {
		int drawCnt = 0;

		boardCpy(&child, pBoard);
		doMove(&child, &mliste._sMoveList[ii]);

		if (pastMoves)
		{
			moveCpy(&pastMoves->_sMoveList[pastMoves->_nbrMove], &mliste._sMoveList[ii]);
			pastMoves->_nbrMove++;
			// We look for draw
			for (drawCnt = 0; drawCnt < 5; drawCnt += 1)
			{
				int m1, m2;
				m1 = pastMoves->_sMoveList[pastMoves->_nbrMove - (1 + drawCnt)]._move;
				m2 = pastMoves->_sMoveList[pastMoves->_nbrMove - (5 + drawCnt)]._move;
				if (m1 != m2)
				{
					break;
				}
			}
		}

		if (drawCnt == 4) { // it's a draw!
			value = 0;
		}
		else {
			value = max(value, -negamaxTT(&child, depth + 1, -beta, -alpha, statistics, state, pastMoves, pvMoves));
		}
		if (value > alpha) {
			moveCpy(&pBoard->_bestMove, &mliste._sMoveList[ii]);
			alpha = value;
		}

		if (pastMoves)
		{
			pastMoves->_nbrMove--;
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
		ttSet(pBoard, value, (uint8_t)statistics->maxDepth - depth, flag);
	}

	// alpha is equal to -INF if this pos is always leading to mat. so we peek the first move to avoid leting best move empty
	if ((alpha == -INF) && (mliste._nbrMove > 0))
	{
		moveCpy(&pBoard->_bestMove, &mliste._sMoveList[0]);
	}

	return value;
}


int findPV(sboard * pBoard, int depth, smoveList * pvMove)
{
	sboard Board;
	negaMaxConf stat;
	boardCpy(&Board, pBoard);
	char strMove[10];

	moveListInit(pvMove);

	stat.maxSearchTime = 5000;
	ftime(&stat.startSearchTIme);

	for (int ii = 1; ii < depth; ii++)
	{
		stat.maxDepth = ii;
		negamaxTT(&Board, 1, -INF, INF, &stat, SEARCH_FIRST, 0, 0);
		doMove(&Board, &Board._bestMove);

		moveCpy(&pvMove->_sMoveList[pvMove->_nbrMove], &Board._bestMove);
		pvMove->_nbrMove++;

		movePrintShort(&Board._bestMove, strMove);
		printTcp(strMove);

	}
	printTcp("\n");
	return 0;
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
smove searchStart(sboard * pBoard, int wtime, int btime, int winc, int binc, int moveToGo, negaMaxConf * stat, smoveList * pastMoves) {
	smoveList pvMove;
	smoveList bookMovesList;
	smove bestMove;
	int incr = 0;
	int player_time = 0;
	char fen[128] = "";
	char str[128] = "";

	moveListInit(&pvMove);
	boardGenerateAllLegalMoves(pBoard, &bookMovesList);

	boardPrintFen(pBoard, &fen);
	printf(fen);

	if (polyglot_listMove(polyglot_hash(&fen), &bestMove))
		return bestMove;



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
	{
		moveToGo = 70 - pastMoves->_nbrMove;
		if (moveToGo < 20)
			moveToGo = 20;
	}


	player_time = player_time / moveToGo;

	stat->maxSearchTime = (player_time + incr);

	snprintf(str, 128, "Search time: %i ms\n", stat->maxSearchTime);
	printTcp(str);

	gStopSearch = 0;
	stat->nbrNode = 0;
	stat->nbrCut = 0;
	stat->nbrZob = 0;
	stat->nbrQuies = 0;

	ftime(&stat->startSearchTIme);

	gNodeCptCheckTime = 0;

	moveOrderClearKiller();


	for (int depth = 3; depth < MAX_SEARCH_DEPTH; depth++) {

		stat->maxDepth = depth;

		if (depth < 5) {
			stat->boardEval = negamaxTT(pBoard, 1, -INF, INF, stat, SEARCH_FIRST, pastMoves, &pvMove);
		}
		else {
			int alpha = stat->boardEval - 100;
			int beta = stat->boardEval + 100;
			int value = negamaxTT(pBoard, 1, alpha, beta, stat, SEARCH_FIRST, pastMoves, &pvMove);
			if (value <= alpha || value >= beta)
			{
				value = negamaxTT(pBoard, 1, -INF, INF, stat, SEARCH_FIRST, pastMoves, &pvMove);
			}
			stat->boardEval = value;
		}

		UciInfo(depth, stat->nbrNode, stat->boardEval, stat);

		findPV(pBoard, depth, &pvMove);

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



