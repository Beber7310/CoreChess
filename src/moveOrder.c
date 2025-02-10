/*
 * moveOrder.c
 *
 *  Created on: Aug 27, 2018
 *      Author: dosdab
 */

#ifdef _WIN32
#include <windows.h>
#endif

#include "board.h"
#include "search.h"
#include "move.h"
#include "movegen.h"

#include "bitutils.h"
#include <string.h>

#define ORDER_NBR_KILLER_MOVE		20
#define ORDER_NBR_KILLER_MOVE_DIM	2

static smove killerMoves[ORDER_NBR_KILLER_MOVE][ORDER_NBR_KILLER_MOVE_DIM];

void moveOrderClearKiller(void) {
	//ZeroMemory(killerMoves, sizeof(killerMoves));
	memset(killerMoves, NULL, sizeof(killerMoves));

}

void moveOrderDebug(smoveList* pMoveList1, smoveList* pMoveList2) {
	int found = 0;

	for (int ii = 0; ii < pMoveList1->_nbrMove; ii++) {
		found = 0;
		for (int jj = 0; jj < pMoveList2->_nbrMove; jj++) {
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

int compare(const void* a, const void* b)
{
	smove* mv_a = ((smove*)a);
	smove* mv_b = ((smove*)b);

	if (mv_a->_value == mv_b->_value) return 0;
	else if (mv_a->_value < mv_b->_value) return -1;
	else return 1;
}

void moveOrder(smoveList* pMoveList, int depth, int filterQuies, smoveList* pvMoves, negaMaxConf* pStat) {
	//	return;

	smoveList tmpList;
	smoveList tmpListA;
	smoveList tmpListB;
	smoveList tmpListC;
	smoveList tmpList0;

	tmpList._nbrMove = 0;
	tmpListA._nbrMove = 0;
	tmpListB._nbrMove = 0;
	tmpListC._nbrMove = 0;
	tmpList0._nbrMove = 0;

	if (pvMoves)
	{
		for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
			if (pMoveList->_sMoveList[ii]._move == pvMoves->_sMoveList[depth - 1]._move) {
				moveCpy(&tmpList._sMoveList[tmpList._nbrMove], &pvMoves->_sMoveList[depth - 1]);
				tmpList._nbrMove++;

				// Remove the move from the original move to avoid to get it 2 times
				moveCpy(&pMoveList->_sMoveList[ii], &pMoveList->_sMoveList[pMoveList->_nbrMove - 1]);
				pMoveList->_nbrMove--;
				break;
			}
		}
	}

	if (filterQuies == 0)
	{
		for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
			for (int dim = 0; dim < ORDER_NBR_KILLER_MOVE_DIM; dim++) {
				for (int jj = depth - 1; (jj < ORDER_NBR_KILLER_MOVE) && (jj < depth + 1); jj++) {
					//for (int jj = 0; jj < ORDER_NBR_KILLER_MOVE; jj++) {
					if (pMoveList->_sMoveList[ii]._move == killerMoves[jj][dim]._move) {
						moveCpy(&tmpList._sMoveList[tmpList._nbrMove], &killerMoves[jj][dim]);
						tmpList._nbrMove++;

						// Remove the move from the original move to avoid to get it 2 times
						moveCpy(&pMoveList->_sMoveList[ii], &pMoveList->_sMoveList[pMoveList->_nbrMove - 1]);
						pMoveList->_nbrMove--;
						ii--;
						dim = ORDER_NBR_KILLER_MOVE_DIM + 1;
						jj = ORDER_NBR_KILLER_MOVE + 1;
					}
				}
			}
		}
	}
	/*
	for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
		if (MOVE_FLAG(pMoveList->_sMoveList[ii]._move) & CAPTURE) {
			moveCpy(&tmpList._sMoveList[tmpList._nbrMove], &pMoveList->_sMoveList[ii]);
			tmpList._nbrMove++;

			// Remove the move from the original move to avoid to get it 2 times
			moveCpy(&pMoveList->_sMoveList[ii], &pMoveList->_sMoveList[pMoveList->_nbrMove - 1]);
			pMoveList->_nbrMove--;
			ii--;
		}
	}
	*/
	if (filterQuies == 0)
	{
		for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
			if (MOVE_FLAG(pMoveList->_sMoveList[ii]._move) & CAPTURE) {

				if (pMoveList->_sMoveList[ii]._value >= 700) {
					moveCpy(&tmpListA._sMoveList[tmpListA._nbrMove++], &pMoveList->_sMoveList[ii]);
				}
				else if (pMoveList->_sMoveList[ii]._value >= 400) {
					moveCpy(&tmpListB._sMoveList[tmpListB._nbrMove++], &pMoveList->_sMoveList[ii]);
				}
				else if (pMoveList->_sMoveList[ii]._value >= 100) {
					moveCpy(&tmpListC._sMoveList[tmpListC._nbrMove++], &pMoveList->_sMoveList[ii]);
				}
				else {
					moveCpy(&tmpList0._sMoveList[tmpList0._nbrMove++], &pMoveList->_sMoveList[ii]);
				}
			}
			else {
				moveCpy(&tmpList0._sMoveList[tmpList0._nbrMove++], &pMoveList->_sMoveList[ii]);
			}
			// Remove the move from the original move to avoid to get it 2 times
			moveCpy(&pMoveList->_sMoveList[ii], &pMoveList->_sMoveList[pMoveList->_nbrMove - 1]);
			pMoveList->_nbrMove--;
			ii--;
		}


	}

	pMoveList->_nbrMove = 0;

	// copy back sorted movement
	for (int ii = 0; ii < tmpList._nbrMove; ii++) {
		moveCpy(&pMoveList->_sMoveList[pMoveList->_nbrMove++], &tmpList._sMoveList[ii]);
	}
	for (int ii = 0; ii < tmpListA._nbrMove; ii++) {
		moveCpy(&pMoveList->_sMoveList[pMoveList->_nbrMove++], &tmpListA._sMoveList[ii]);
	}
	for (int ii = 0; ii < tmpListB._nbrMove; ii++) {
		moveCpy(&pMoveList->_sMoveList[pMoveList->_nbrMove++], &tmpListB._sMoveList[ii]);
	}
	for (int ii = 0; ii < tmpListC._nbrMove; ii++) {
		moveCpy(&pMoveList->_sMoveList[pMoveList->_nbrMove++], &tmpListC._sMoveList[ii]);
	}
	for (int ii = 0; ii < tmpList0._nbrMove; ii++) {
		moveCpy(&pMoveList->_sMoveList[pMoveList->_nbrMove++], &tmpList0._sMoveList[ii]);
	}
}

void moveOrderAddKiller(smove* pMove, int depth) {
	if (depth < ORDER_NBR_KILLER_MOVE) {
		for (int ii = 0; ii < ORDER_NBR_KILLER_MOVE_DIM; ii++)
		{
			if (killerMoves[depth][ii]._move == 0)
			{
				killerMoves[depth][ii]._move = pMove->_move;
				killerMoves[depth][ii]._value = abs(pMove->_value);
				return;
			}
			else if (killerMoves[depth][ii]._move == pMove->_move)
			{
				killerMoves[depth][ii]._move = pMove->_move;
				killerMoves[depth][ii]._value = max(abs(pMove->_value), killerMoves[depth][ii]._value);
				return;
			}
			else if (killerMoves[depth][ii]._value < abs(pMove->_value))
			{
				killerMoves[depth][ii]._move = pMove->_move;
				killerMoves[depth][ii]._value = abs(pMove->_value);
				return;
			}
			/*	else {
					killerMoves[depth][ii]._value -= 1;
					if (killerMoves[depth][ii]._value < 100)
					{
						killerMoves[depth][ii]._move = 0;
						killerMoves[depth][ii]._value = 0;
					}
				}*/
		}
	}
}
