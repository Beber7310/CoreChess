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

#define ORDER_NBR_KILLER_MOVE	20
static smove killerMoves[ORDER_NBR_KILLER_MOVE];

void moveOrderClearKiller(void) {
	for (int ii = 0; ii < ORDER_NBR_KILLER_MOVE; ii++) {
		killerMoves[ii]._move = 0;
		killerMoves[ii]._value = 0;
	}
}

void moveOrderDebug(smoveList* pMoveList1, smoveList* pMoveList2) {
	int found = 0;
	for (int ii = 0; ii < pMoveList1->_nbrMove; ii++) {
		found = 0;
		for (int jj = 0; jj < pMoveList1->_nbrMove; jj++) {
			if (pMoveList1->_sMoveList[ii]._move == pMoveList2->_sMoveList[jj]._move) {
				found = 1;
			}

		}
		if (found == 0) {
			printf("moveOrderDebug: Erreur\n");
			exit(-1);
		}
	}
}

void moveOrder(smoveList* pMoveList, int depth, searchStat* pStat) {
	int idx = 0;
	smoveList tmpList;

	for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
		if (pMoveList->_sMoveList[ii]._move == killerMoves[depth]._move) {
			if (pMoveList->_sMoveList[ii]._value < killerMoves[depth]._value) {
				moveCpy(&tmpList._sMoveList[idx], &killerMoves[depth]);
				idx++;

				// Remove the move from the original move to avoid to get it 2 times
				moveCpy(&pMoveList->_sMoveList[ii], &pMoveList->_sMoveList[pMoveList->_nbrMove-1]);
				pMoveList->_nbrMove--;
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

	moveOrderDebug(pMoveList,&tmpList);
}



void moveOrderAddKiller(smove* pMove, int depth) {
	if (depth < ORDER_NBR_KILLER_MOVE) {
		killerMoves[depth]._move = pMove->_move;
		killerMoves[depth]._value = pMove->_value;
	}
}
