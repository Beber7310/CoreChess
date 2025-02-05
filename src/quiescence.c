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

extern int gStopSearch;			// used to stop digging when time is over
extern int gNodeCptCheckTime;		//Used as a counter to choose when to print some info
extern int uciOptionQuiesence;
 
int Quiessence(sboard* pBoard, int depth, int alpha, int beta, negaMaxConf* statistics, search_state state) {
	smoveList mliste;
	sboard child;
	 
	if (gStopSearch)
		return 0;
 
	int val;
	if (pBoard->_ActivePlayer == WHITE)
		val = evaluate(pBoard);
	else
		val = -evaluate(pBoard);

	if (depth < 5)
		return val;

	int stand_pat = val;

	/* check if stand-pat score causes a beta cutoff */
	if (val >= beta)
		return beta;

	/* check if stand-pat score may become a new alpha */
	if (alpha < val)
		alpha = val;

	 
	boardGenerateAllLegalMoves(pBoard, &mliste);
	// Check for checkmate and stalemate
	if (mliste._nbrMove == 0) {
		statistics->nbrNode++;
		if (colorIsInCheck(pBoard, pBoard->_ActivePlayer)) {
			return -INF;
		}
		return 0; // mate
	}

	/*if (depth == 0) { // or node is a terminal node then
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
	*/

	moveOrder(&mliste, 0, 1, statistics); 

	int value = -INF;
	for (int ii = 0; ii < mliste._nbrMove; ii++) {

		if (MOVE_PIECE_CAPTURED(mliste._sMoveList[ii]._move) == KING)
			return INF;


		boardCpy(&child, pBoard);
		doMove(&child, &mliste._sMoveList[ii]);

		value = -Quiessence(&child, depth - 1, -beta, -alpha, statistics, state);

		if (value > alpha) {		
			if (value >= beta) {								
				moveOrderAddKiller(&mliste._sMoveList[ii], 0);
				return beta;
			}
			alpha = value;
		}
	}
	return alpha;

}
 //setoption name Quiescence value true
