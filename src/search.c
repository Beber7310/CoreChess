/*
 * search.c
 *
 *  Created on: 21 août 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "search.h"
#include "evaluate.h"

#define INF (99999)

int max(int a, int b) {
	if (a > b)
		return a;
	else
		return b;
}

smove searchStart(sboard * pBoard, int depth) {
	negamax(pBoard, depth, -INF, INF, pBoard->_ActivePlayer);
	return pBoard->_bestMove;
}

int negamax(sboard * pNode, int depth, int alpha, int beta, Color color) {
	smoveList mliste;
	sboard child;
	if (depth == 0) { // or node is a terminal node then
		int res = evaluate(pNode);
		if (color == WHITE)
			return res;
		else
			return -res;
	}

	boardGenerateAllLegalMoves(pNode, &mliste);	// childNodes := generateMoves(node)
												// childNodes := orderMoves(childNodes)
	int value = -INF;
	for (int ii = 0; ii < mliste._nbrMove; ii++) {
		boardCpy(&child, pNode);

		doMove(&child, &mliste._sMoveList[ii]);
		value = max(value, -negamax(&child, depth - 1, -beta, -alpha, !color));
		if (value > alpha)
			moveCpy(&pNode->_bestMove,  &mliste._sMoveList[ii]);
		alpha = max(alpha, value);
		if (alpha >= beta) {
			return value;	//break (* cut-off *)
		}
	}
	return value;
}
