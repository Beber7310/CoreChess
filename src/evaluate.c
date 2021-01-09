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

int knight_pcsq_mg[64] = {
	-8,  -8,  -8,  -8,  -8,  -8,  -8,  -8,
	-8,   0,   0,   0,   0,   0,   0,  -8,
	-8,   0,   4,   6,   6,   4,   0,  -8,
	-8,   0,   6,   8,   8,   6,   0,  -8,
	-8,   0,   6,   8,   8,   6,   0,  -8,
	-8,   0,   4,   6,   6,   4,   0,  -8,
	-8,   0,   1,   2,   2,   1,   0,  -8,
   -16, -12,  -8,  -8,  -8,  -8, -12,  -16
};

int bishop_pcsq_mg[64] = {
	-4,  -4,  -4,  -4,  -4,  -4,  -4,  -4,
	-4,   0,   0,   0,   0,   0,   0,  -4,
	-4,   0,   2,   4,   4,   2,   0,  -4,
	-4,   0,   4,   6,   6,   4,   0,  -4,
	-4,   0,   4,   6,   6,   4,   0,  -4,
	-4,   1,   2,   4,   4,   2,   1,  -4,
	-4,   2,   1,   1,   1,   1,   2,  -4,
	-4,  -4, -12,  -4,  -4, -12,  -4,  -4
};

int rook_pcsq_mg[64] = {
	 5,   5,   5,   5,   5,   5,   5,   5,
	-5,   0,   0,   0,   0,   0,   0,  -5,
	-5,   0,   0,   0,   0,   0,   0,  -5,
	-5,   0,   0,   0,   0,   0,   0,  -5,
	-5,   0,   0,   0,   0,   0,   0,  -5,
	-5,   0,   0,   0,   0,   0,   0,  -5,
	-5,   0,   0,   0,   0,   0,   0,  -5,
	 0,   0,   0,   2,   2,   0,   0,   0
};

int pawn_pcsq_mg[64] = {
	 0,   0,   0,   0,   0,   0,   0,   0,
	-6,  -4,   1,   1,   1,   1,  -4,  -6,
	-6,  -4,   1,   2,   2,   1,  -4,  -6,
	-6,  -4,   2,   8,   8,   2,  -4,  -6,
	-6,  -4,   5,  10,  10,   5,  -4,  -6,
	-4,  -4,   1,   5,   5,   1,  -4,  -4,
	-6,  -4,   1, -24,  -24,  1,  -4,  -6,
	 0,   0,   0,   0,   0,   0,   0,   0
};

int queen_pcsq_mg[64] = {
	 0,   0,   0,   0,   0,   0,   0,   0,
	 0,   0,   1,   1,   1,   1,   0,   0,
	 0,   0,   1,   2,   2,   1,   0,   0,
	 0,   0,   2,   3,   3,   2,   0,   0,
	 0,   0,   2,   3,   3,   2,   0,   0,
	 0,   0,   1,   2,   2,   1,   0,   0,
	 0,   0,   1,   1,   1,   1,   0,   0,
	-5,  -5,  -5,  -5,  -5,  -5,  -5,  -5
};

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

	return (attacks & ~own);

}


 int evaluateKnight(sboard* pboard)
 {
	 int score = 0;
	 U64 knights = pboard->_pieces[WHITE][KNIGHT];
	 while (knights) {
		 int to = _popLsb(&knights);
		 score += knight_pcsq_mg[63 - to];
	 }

	 knights = pboard->_pieces[BLACK][KNIGHT];
	 while (knights) {
		 int to = _popLsb(&knights);
		 score -= knight_pcsq_mg[to];
	 }

 }

 int evaluateQueen(sboard* pboard)
 {
	 int score = 0;
	 U64 pawn = pboard->_pieces[WHITE][QUEEN];
	 while (pawn) {
		 int to = _popLsb(&pawn);
		 score += queen_pcsq_mg[63 - to];
	 }

	 pawn = pboard->_pieces[BLACK][QUEEN];
	 while (pawn) {
		 int to = _popLsb(&pawn);
		 score -= queen_pcsq_mg[to];
	 }
 }

 int evaluatePawn(sboard* pboard)
 {
	 int score = 0;
	 U64 pawn = pboard->_pieces[WHITE][PAWN];
	 while (pawn) {
		 int to = _popLsb(&pawn);
		 score += pawn_pcsq_mg[63 - to];
	 }

	 pawn = pboard->_pieces[BLACK][PAWN];
	 while (pawn) {
		 int to = _popLsb(&pawn);
		 score -= pawn_pcsq_mg[to];
	 }
 }

 int evaluateBishop(sboard* pboard)
 {
	 int score = 0;
	 U64 bishop = pboard->_pieces[WHITE][BISHOP];
	 while (bishop) {
		 int to = _popLsb(&bishop);
		 score += bishop_pcsq_mg[63 - to];
	 }

	 bishop = pboard->_pieces[BLACK][BISHOP];
	 while (bishop) {
		 int to = _popLsb(&bishop);
		 score -= bishop_pcsq_mg[to];
	 }
 }


 int evaluateRook(sboard* pboard)
 {
	 int score = 0;
	 U64 rook = pboard->_pieces[WHITE][ROOK];
	 while (rook) {
		 int to = _popLsb(&rook);
		 score += rook_pcsq_mg[63 - to];
	 }

	 rook = pboard->_pieces[BLACK][ROOK];
	 while (rook) {
		 int to = _popLsb(&rook);
		 score -= rook_pcsq_mg[to];
	 }
 }

int evaluate(sboard * pboard) {
	int score = 0;
	U64 attacksW = ZERO;
	U64 attacksB = ZERO;



	// Material value
	score += PAWN_VALUE * (_popCount(pboard->_pieces[WHITE][PAWN]) - _popCount(pboard->_pieces[BLACK][PAWN]));
	score += KNIGHT_VALUE * (_popCount(pboard->_pieces[WHITE][KNIGHT]) - _popCount(pboard->_pieces[BLACK][KNIGHT]));
	score += BISHOP_VALUE * (_popCount(pboard->_pieces[WHITE][BISHOP]) - _popCount(pboard->_pieces[BLACK][BISHOP]));
	score += ROOK_VALUE * (_popCount(pboard->_pieces[WHITE][ROOK]) - _popCount(pboard->_pieces[BLACK][ROOK]));
	score += QUEEN_VALUE * (_popCount(pboard->_pieces[WHITE][QUEEN]) - _popCount(pboard->_pieces[BLACK][QUEEN]));

	if(_popCount(pboard->_pieces[WHITE][BISHOP]==2))
		score += BISHOP_PAIR;

	if (_popCount(pboard->_pieces[BLACK][BISHOP] == 2))
		score -= BISHOP_PAIR;

	if (_popCount(pboard->_pieces[WHITE][KNIGHT] == 2))
		score += KNIGHT_PAIR;

	if (_popCount(pboard->_pieces[BLACK][KNIGHT] == 2))
		score -= KNIGHT_PAIR;
	
	if (_popCount(pboard->_pieces[WHITE][ROOK] == 2))
		score += ROOK_PAIR;

	if (_popCount(pboard->_pieces[BLACK][ROOK] == 2))
		score -= ROOK_PAIR;

	 
	attacksW = evaluateMobility(pboard, WHITE);
	attacksB = evaluateMobility(pboard, BLACK);

	score += MOBILITY_BONUS * (_popCount(attacksW) - _popCount(attacksB));
	score += ATTACK_BONUS * (_popCount(attacksW & pboard->_allPieces[BLACK]) - _popCount(attacksB & pboard->_allPieces[WHITE]));

	score += evaluateKnight(pboard);
	score += evaluateBishop(pboard);
	score += evaluateRook(pboard);
	score += evaluatePawn(pboard);
		
	return score;
}
