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

extern int gStopSearch;		  // used to stop digging when time is over
extern int gNodeCptCheckTime; //Used as a counter to choose when to print some info
 

#define INF (99999)
  

int negamaxTT(sboard* pNode, int depth, int alpha, int beta, Color color, searchStat* stat,int first) {
	smoveList mliste;	
	sboard child;
	ttEntry* tt;

	int alphaOrig = alpha;

	if (gStopSearch)
		return 0;
	 
	if (depth != 0 && !first)
	{
		tt = ttGet(pNode->_zobKey);
		if (tt) {
			if (tt->flag != EMPTY) {
				if (tt->depth >= (depth)) {
					stat->nbrZob++;
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

		if (color != pNode->_ActivePlayer)
			printf("error in negamaxTT arround color\n");

		if (pNode->_ActivePlayer == WHITE)
			return res;
		else
			return -res;
	}

	 

	moveOrder(&mliste, depth, stat);

	int value = -INF;
	for (int ii = 0; ii < mliste._nbrMove; ii++) {

		

		boardCpy(&child, pNode);
		doMove(&child, &mliste._sMoveList[ii]);
	
		value = max(value, -negamaxTT(&child, depth - 1, -beta, -alpha, !color, stat,0));
				 
		if (value > alpha) {
			moveCpy(&pNode->_bestMove, &mliste._sMoveList[ii]);			
			alpha = value;
		}

		if (alpha >= beta) {
			stat->nbrCut++;
			mliste._sMoveList[ii]._value = alpha;
			moveOrderAddKiller(&mliste._sMoveList[ii], depth);
			break;  //break (* cut-off *)
		}

	}

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

	ttSet(pNode, value, depth, flag);


	// alpha is equal to -INF if this pos is always leading to mat. so we peek the first move to avoid leting best move empty
	if ((alpha == -INF) && (mliste._nbrMove > 0))
	{
		moveCpy(&pNode->_bestMove, &mliste._sMoveList[0]);
	}



	return value;
}

