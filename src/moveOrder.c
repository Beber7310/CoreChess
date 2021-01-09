/*
 * moveOrder.c
 *
 *  Created on: Aug 27, 2018
 *      Author: dosdab
 */
#include <windows.h>

#include "board.h"
#include "search.h"
#include "move.h"
#include "movegen.h"

#include "bitutils.h"
#include <string.h>

#define ORDER_NBR_KILLER_MOVE		20
#define ORDER_NBR_KILLER_MOVE_DIM	5

static smove killerMoves[ORDER_NBR_KILLER_MOVE][ORDER_NBR_KILLER_MOVE_DIM];

void moveOrderClearKiller(void) {
	/*for (int ii = 0; ii < ORDER_NBR_KILLER_MOVE; ii++) {
		for (int jj = 0; jj < ORDER_NBR_KILLER_MOVE_DIM; jj++) {
			killerMoves[ii][jj]._move = 0;
			killerMoves[ii][jj]._value = 0;
		}
	}*/
	ZeroMemory(killerMoves, sizeof(killerMoves));
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

void moveOrder(smoveList* pMoveList, int depth, int filterQuies, searchStat* pStat) {
	//	return;

	int idx = 0;
	int initialNbrMv = pMoveList->_nbrMove;
	smoveList tmpList;
	tmpList._nbrMove = 0;

	if (filterQuies == 0)
	{
		for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
			for (int dim = 0; dim < ORDER_NBR_KILLER_MOVE_DIM; dim++) {
				for (int jj = 0; jj < ORDER_NBR_KILLER_MOVE; jj++) {
					if (pMoveList->_sMoveList[ii]._move == killerMoves[jj][dim]._move) {
						moveCpy(&tmpList._sMoveList[idx], &killerMoves[jj][dim]);
						tmpList._nbrMove++;
						idx++;
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

	if (filterQuies == 0)
	{
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
	}

	pMoveList->_nbrMove = tmpList._nbrMove;

	// copy back sorted movement
	for (int ii = 0; ii < pMoveList->_nbrMove; ii++) {
		moveCpy(&pMoveList->_sMoveList[ii], &tmpList._sMoveList[ii]);
	}

#ifdef _DEBUG	
	if (filterQuies == 0)
	{
		if (initialNbrMv != idx)
		{
			printf("Error in moveOrder: some move are not copied in tmp moves 2\n");
		}
		if (initialNbrMv != tmpList._nbrMove)
		{
			printf("Error in moveOrder: some move are not copied in tmp moves 2\n");
		}

		moveOrderDebug(pMoveList, &tmpList);
		moveOrderDebug(&tmpList, pMoveList);
	}
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
