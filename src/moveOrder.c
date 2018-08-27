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

void moveOrder(smoveList* pMoveList, searchStat* pStat) {
	int idx = 0;
	smoveList tmpList;
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
	for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
		moveCpy(&pMoveList->_sMoveList[ii],&tmpList._sMoveList[ii]);
	}
}
