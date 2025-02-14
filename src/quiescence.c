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

extern int gStopSearch;			// used to stop digging when time is over
extern int gNodeCptCheckTime;		//Used as a counter to choose when to print some info
extern int uciOptionQuiesence;

// int Quiessence(sboard* pBoard, int depth, int alpha, int beta, negaMaxConf* statistics, search_state state);

int Quiessence(sboard* pBoard, int depth, int alpha, int beta, negaMaxConf* statistics, search_state state) {
	smoveList mliste;
	sboard child;
	ttEntry* tt;

	//int alphaOrig = alpha;

	int stand_pat = evaluate(pBoard);
	int best_value = stand_pat;
	statistics->nbrQuies++;

	if (stand_pat >= beta)
		return stand_pat;
	if (alpha < stand_pat)
		alpha = stand_pat;

	if (gStopSearch)
		return 0;

	boardGenerateAllLegalMoves(pBoard, &mliste);

	if (mliste._nbrMove == 0) {				// Check for checkmate and stalemate
		statistics->nbrNode++;
		if (colorIsInCheck(pBoard, pBoard->_ActivePlayer)) {
			return -INF;
		}
		return 0; // mate
	}

	moveOrder(&mliste, depth, state == SEARCH_QUIESSENCE, NULL, statistics);

	if (depth < statistics->maxDepth + 2) {
		int score = -INF;
		for (int ii = 0; ii < mliste._nbrMove; ii++) {
			int drawCnt = 0;

			boardCpy(&child, pBoard);
			doMove(&child, &mliste._sMoveList[ii]);

			score = -Quiessence(&child, depth + 1, -beta, -alpha, statistics, state);

			if (score >= beta)
				return score;
			if (score > best_value)
				best_value = score;
			if (score > alpha)
				alpha = score;

		}
	}
	return best_value;
}

