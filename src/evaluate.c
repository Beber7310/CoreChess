/*
 * evaluate.c
 *
 *  Created on: 22 août 2018
 *      Author: Bertrand
 */
#include "board.h"
#include "evaluate.h"
#include "bitutils.h"
#include "move.h"
#include "movegen.h"


//position startpos moves b2b4 a7a6 b4b5

int evaluateMobility(sboard * pBoard, Color color) {

	U64 own = pBoard->_allPieces[color];
	U64 attacks = ZERO;

	PieceType ptype = 0;

	while (ptype < 6) {
		U64 ptm = pBoard->_pieces[color][ptype];
		while (ptm) {
			int from = _popLsb(&ptm);
			switch (ptype) {
			case ROOK:
				attacks |= getRookAttacks(from, pBoard->_occupied) & ~pBoard->_allPieces[color];
				break;
			case KNIGHT:
				attacks |= getKnightAttacks(from) & ~pBoard->_allPieces[color];
				break;
			case BISHOP:
				attacks |= getBishopAttacks(from, pBoard->_occupied) & ~pBoard->_allPieces[color];
				break;
			case QUEEN:
				attacks |= getQueenAttacks(from, pBoard->_occupied) & ~pBoard->_allPieces[color];
				break;
			case KING:
				attacks |= getKingAttacks(from) & ~pBoard->_allPieces[color];
				break;
			case PAWN:
				break;
			}
		}
		ptype++;
	}

	return _popCount(attacks & ~own);

}

int evaluate(sboard * pboard) {
	int score = 0;

	Color color = WHITE; //pboard->_ActivePlayer;
	Color otherColor = BLACK; // !pboard->_ActivePlayer;

	// Material value
	score += PAWN_VALUE * (_popCount(pboard->_pieces[color][PAWN]) - _popCount(pboard->_pieces[otherColor][PAWN]));
	score += KNIGHT_VALUE * (_popCount(pboard->_pieces[color][KNIGHT]) - _popCount(pboard->_pieces[otherColor][KNIGHT]));
	score += BISHOP_VALUE * (_popCount(pboard->_pieces[color][BISHOP]) - _popCount(pboard->_pieces[otherColor][BISHOP]));
	score += ROOK_VALUE * (_popCount(pboard->_pieces[color][ROOK]) - _popCount(pboard->_pieces[otherColor][ROOK]));
	score += QUEEN_VALUE * (_popCount(pboard->_pieces[color][QUEEN]) - _popCount(pboard->_pieces[otherColor][QUEEN]));

	score += MOBILITY_BONUS * (evaluateMobility(pboard,color)-evaluateMobility(pboard,otherColor));
	return score;
}

