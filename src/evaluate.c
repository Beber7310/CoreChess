/*
 * evaluate.c
 *
 *  Created on: 22 août 2018
 *      Author: Bertrand
 */
#include "board.h"
#include "evaluate.h"
#include "bitutils.h"


int evaluate(sboard * pboard) {
	int score = 0;

	Color color = pboard->_ActivePlayer;
	Color otherColor = !pboard->_ActivePlayer;

	// Material value
	score += PAWN_VALUE   * (_popCount(pboard->_pieces[color][PAWN])   - _popCount(pboard->_pieces[otherColor][PAWN]));
	score += KNIGHT_VALUE * (_popCount(pboard->_pieces[color][KNIGHT]) - _popCount(pboard->_pieces[otherColor][KNIGHT]));
	score += BISHOP_VALUE * (_popCount(pboard->_pieces[color][BISHOP]) - _popCount(pboard->_pieces[otherColor][BISHOP]));
	score += ROOK_VALUE   * (_popCount(pboard->_pieces[color][ROOK])   - _popCount(pboard->_pieces[otherColor][ROOK]));
	score += QUEEN_VALUE  * (_popCount(pboard->_pieces[color][QUEEN])  - _popCount(pboard->_pieces[otherColor][QUEEN]));

	return score;
}

