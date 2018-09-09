/*
 * zobrist .c
 *
 *  Created on: 25 août 2018
 *      Author: Bertrand
 */
#include "zobrist.h"

#include <stdlib.h>
#include "board.h"
#include "bitutils.h"

U64 PIECE_KEYS[2][6][64];
U64 EN_PASSANT_KEYS[8];
U64 KS_CASTLE_KEYS[2];
U64 QS_CASTLE_KEYS[2];
U64 WHITE_TO_MOVE_KEY;

U64 rand64(void) {

	U64 ret = 0;

	for (int ii = 0; ii < 8; ii++) {
		ret <<= 8;
		ret |= rand() % 256;
	}
	return ret;

}

void zobInit(void) {
	for (int pieceType = 0; pieceType < 6; pieceType++) {
		for (int square = 0; square < 64; square++) {
			PIECE_KEYS[WHITE][pieceType][square] = rand64();
			PIECE_KEYS[BLACK][pieceType][square] = rand64();
		}
	}

	for (int square = 0; square < 8; square++) {
		EN_PASSANT_KEYS[square] = rand64();
	}

	KS_CASTLE_KEYS[WHITE] = rand64();
	KS_CASTLE_KEYS[BLACK] = rand64();

	QS_CASTLE_KEYS[WHITE] = rand64();
	QS_CASTLE_KEYS[BLACK] = rand64();

	WHITE_TO_MOVE_KEY = rand64();

}

U64 zobFlipPiece(Color color, PieceType piece, unsigned int index, U64 key) {
	key ^= PIECE_KEYS[color][piece][index];
	return key;
}

U64 zobFlipKsCastle(Color color, U64 key) {
	key ^= KS_CASTLE_KEYS[color];
	return key;
}

U64 zobFlipQsCastle(Color color, U64 key) {
	key ^= QS_CASTLE_KEYS[color];
	return key;
}

U64 zobFlipPLayer(U64 key) {
	return key ^ WHITE_TO_MOVE_KEY;
}

U64 zobEnPassant(U64 key, unsigned int file) {
	return key ^ EN_PASSANT_KEYS[file];
}

U64 zobCompute(sboard* board) {
	U64 _key = ZERO;

	if (board->_ActivePlayer == WHITE) {
		_key ^= WHITE_TO_MOVE_KEY;
	}

	// Add white/black pieces
	for (int piece = 0; piece < 6; piece++) {
		U64 whiteBitBoard = board->_pieces[WHITE][piece];
		U64 blackBitBoard = board->_pieces[BLACK][piece];

		for (int squareIndex = 0; squareIndex < 64; squareIndex++) {
			U64 square = ONE << squareIndex;
			if (square & whiteBitBoard) {
				_key = zobFlipPiece(WHITE, piece, squareIndex, _key);
			} else if (square & blackBitBoard) {
				_key = zobFlipPiece(BLACK, piece, squareIndex, _key);
			}
		}
	}

	// Add en passant
	if (board->_enPassant) {
		_key = zobEnPassant(_key, board->_enPassant % 8);
	}

	// Add castles
	if (board->_castlingRights & CASTLING_WHITE_KING) { //getKsCastlingRights(WHITE)
		_key = zobFlipKsCastle(WHITE, _key);
	}
	if (board->_castlingRights & CASTLING_WHITE_QUEEN) { //getQsCastlingRights(WHITE)
		_key = zobFlipQsCastle(WHITE, _key);
	}
	if (board->_castlingRights & CASTLING_BLACK_KING) { //getKsCastlingRights(BLACK)
		_key = zobFlipKsCastle(BLACK, _key);
	}
	if (board->_castlingRights & CASTLING_BLACK_QUEEN) { // getQsCastlingRights(BLACK)
		_key = zobFlipQsCastle(BLACK, _key);
	}

	return _key;
}
