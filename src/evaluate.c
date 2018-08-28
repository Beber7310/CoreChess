/*
 * evaluate.c
 *
 *  Created on: 22 août 2018
 *      Author: Bertrand
 */
#include "board.h"
#include "evaluate.h"
#include "bitutils.h"

//position startpos moves b2b4 a7a6 b4b5

int evaluateMobility(sboard * pboard) {
	int score = 0;

	PieceType ptype = 0;

	while (ptype < 6) {
		U64 ptm = pboard->_pieces[pboard->_ActivePlayer][ptype];
		while (ptm) {
			int from = _popLsb(&ptm);
			getMovesForSquare(pboard, moveList, ptype, pboard->_ActivePlayer, from);
		}
		ptype++;
	}

	  U64 getMovesForSquare(sboard * pBoard, smoveList* moveList, PieceType pieceType, Color color, int square) {

	return score;
}

int evaluate(sboard * pboard) {
	int score = 0;

	Color color = WHITE;//pboard->_ActivePlayer;
	Color otherColor = BLACK;// !pboard->_ActivePlayer;

	// Material value
	score += PAWN_VALUE   * (_popCount(pboard->_pieces[color][PAWN])   - _popCount(pboard->_pieces[otherColor][PAWN]));
	score += KNIGHT_VALUE * (_popCount(pboard->_pieces[color][KNIGHT]) - _popCount(pboard->_pieces[otherColor][KNIGHT]));
	score += BISHOP_VALUE * (_popCount(pboard->_pieces[color][BISHOP]) - _popCount(pboard->_pieces[otherColor][BISHOP]));
	score += ROOK_VALUE   * (_popCount(pboard->_pieces[color][ROOK])   - _popCount(pboard->_pieces[otherColor][ROOK]));
	score += QUEEN_VALUE  * (_popCount(pboard->_pieces[color][QUEEN])  - _popCount(pboard->_pieces[otherColor][QUEEN]));



	return score;
}

