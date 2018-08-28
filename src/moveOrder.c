/*
 * moveOrder.c
 *
 *  Created on: Aug 27, 2018
 *      Author: dosdab
 */

#include "board.h"
#include "search.h"
#include "move.h"
#include "movegen.h"

#include "bitutils.h"
#include <string.h>

#define ORDER_NBR_KILLER_MOVE	3
static smove killerMoves[ORDER_NBR_KILLER_MOVE];
static int indexKiller;

void moveOrderInit(void) {
	for (int ii = 0; ii < ORDER_NBR_KILLER_MOVE; ii++) {
		killerMoves[ii]._move = 0;
		killerMoves[ii]._value = 0;
	}
	indexKiller = 0;
}

void moveOrder(smoveList* pMoveList, searchStat* pStat) {
	int idx = 0;
	smoveList tmpList;

	for (int jj = 0; jj < ORDER_NBR_KILLER_MOVE; jj++) {
		for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
			if (pMoveList->_sMoveList[ii]._move == killerMoves[ii]._move) {
				moveCpy(&tmpList._sMoveList[idx], &killerMoves[ii]);
				idx++;
			}
		}
	}

	for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
		if (MOVE_FLAG(pMoveList->_sMoveList[ii]._move) & CAPTURE) {
			moveCpy(&tmpList._sMoveList[idx], &pMoveList->_sMoveList[ii]);
			idx++;
		}
	}
	for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
		if ((MOVE_FLAG(pMoveList->_sMoveList[ii]._move) & CAPTURE) == 0) {
			moveCpy(&tmpList._sMoveList[idx], &pMoveList->_sMoveList[ii]);
			idx++;
		}
	}


	// copy back sorted movement
	for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
		moveCpy(&pMoveList->_sMoveList[ii], &tmpList._sMoveList[ii]);
	}
}

void moveOrderAddKiller(smove* pMove) {
	killerMoves[indexKiller]._move = pMove->_move;
	killerMoves[indexKiller]._value = pMove->_value;
	indexKiller++;
	if(indexKiller>=ORDER_NBR_KILLER_MOVE)
		indexKiller=0;
}
