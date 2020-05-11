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

#define ORDER_NBR_KILLER_MOVE		20
#define ORDER_NBR_KILLER_MOVE_DIM	2

static smove killerMoves[ORDER_NBR_KILLER_MOVE][ORDER_NBR_KILLER_MOVE_DIM];

void moveOrderClearKiller(void) {
	for (int ii = 0; ii < ORDER_NBR_KILLER_MOVE; ii++) {
		for (int jj = 0; jj < ORDER_NBR_KILLER_MOVE; jj++) {
			killerMoves[ii][jj]._move = 0;
			killerMoves[ii][jj]._value = 0;
		}
	}
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

void moveOrder(smoveList* pMoveList, int depth, searchStat* pStat) {
	//	return;

	int idx = 0;
	int initialNbrMv = pMoveList->_nbrMove;
	smoveList tmpList;
	tmpList._nbrMove = 0;
	
	return;

	/*
	for (int jj = 0; jj < ORDER_NBR_KILLER_MOVE_DIM; jj++) {
		//for (int dvar = (depth > 0) ? -1 : 0; dvar < 2; dvar++) {
		for (int dvar = 0 ; dvar < 1; dvar++) {
			for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
				if (pMoveList->_sMoveList[ii]._move == killerMoves[depth + dvar][jj]._move) {
					moveCpy(&tmpList._sMoveList[idx], &killerMoves[depth + dvar][jj]);
					idx++;

					// Remove the move from the original move to avoid to get it 2 times
					if (pMoveList->_nbrMove)
					{
						moveCpy(&pMoveList->_sMoveList[ii], &pMoveList->_sMoveList[pMoveList->_nbrMove - 1]);
						pMoveList->_nbrMove--;
						ii--;
					}
				}
			}
		}
	}
 
 */

	/**/
	for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
		if (MOVE_FLAG(pMoveList->_sMoveList[ii]._move) & CAPTURE) {
			moveCpy(&tmpList._sMoveList[idx], &pMoveList->_sMoveList[ii]);
			tmpList._nbrMove++;
			idx++;

			// Remove the move from the original move to avoid to get it 2 times
			
				moveCpy(&pMoveList->_sMoveList[ii], &pMoveList->_sMoveList[pMoveList->_nbrMove - 1]);
				pMoveList->_nbrMove--;
				ii--;
			
		}
	}

	for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
		if ((MOVE_FLAG(pMoveList->_sMoveList[ii]._move) & CAPTURE) == 0) {
			moveCpy(&tmpList._sMoveList[idx], &pMoveList->_sMoveList[ii]);
			tmpList._nbrMove++;
			idx++;

			// Remove the move from the original move to avoid to get it 2 times
			
			
				moveCpy(&pMoveList->_sMoveList[ii], &pMoveList->_sMoveList[pMoveList->_nbrMove - 1]);
				pMoveList->_nbrMove--;
				ii--;
			
			
		}
	}

#ifdef _DEBUG
	if (pMoveList->_nbrMove != 0)
	{
		printf("Error in moveOrder: some move are not copied in tmp moves 1\n");
	}
#endif


	pMoveList->_nbrMove = initialNbrMv;

	// copy back sorted movement
	for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
		moveCpy(&pMoveList->_sMoveList[ii], &tmpList._sMoveList[ii]);
	}

#ifdef _DEBUG	
	if (initialNbrMv != idx)
	{
		printf("Error in moveOrder: some move are not copied in tmp moves 2\n");
	}
	if (initialNbrMv != tmpList._nbrMove)
	{
		printf("Error in moveOrder: some move are not copied in tmp moves 2\n");
	}

	moveOrderDebug(pMoveList, &tmpList);
	moveOrderDebug( &tmpList, pMoveList);
#endif
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
			if (killerMoves[depth][ii]._move == pMove->_move)
			{
				killerMoves[depth][ii]._move = pMove->_move;
				killerMoves[depth][ii]._value = max(abs(pMove->_value), killerMoves[depth][ii]._value);
				return;
			}
			if (killerMoves[depth][ii]._value < abs(pMove->_value))
			{
				killerMoves[depth][ii]._move = pMove->_move;
				killerMoves[depth][ii]._value = abs(pMove->_value);
				return;
			}
		}
	}
}
