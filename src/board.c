/*
 * board.c
 *
 *  Created on: 7 ao�t 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "movegen.h"
#include "bitutils.h"
#include <string.h>

void boardClear(sboard * pBoard) {
	for (int ii = 0; ii < 2; ii++) {
		for (int jj = 0; jj < 6; jj++) {
			pBoard->_pieces[ii][jj] = ZERO;
		}

		pBoard->_allPieces[WHITE] = ZERO;
		pBoard->_allPieces[BLACK] = ZERO;

		pBoard->_attackable[WHITE] = ZERO;
		pBoard->_attackable[BLACK] = ZERO;
	}

	//_enPassant = ZERO;

	pBoard->_occupied = ZERO;
	pBoard->_notOccupied = ZERO;
	pBoard->_enPassant = ZERO;
	pBoard->_castlingRights = ZERO;

	pBoard->_bestMove._move = 0;
	pBoard->_bestMove._value = 0;
	return;
}

void boardInit(sboard * pBoard) {
	pBoard->_pieces[WHITE][PAWN] = RANK_2;
	pBoard->_pieces[BLACK][PAWN] = RANK_7;

	pBoard->_pieces[WHITE][ROOK] = RANK_1 & (FILE_A | FILE_H);
	pBoard->_pieces[BLACK][ROOK] = RANK_8 & (FILE_A | FILE_H);

	pBoard->_pieces[WHITE][KNIGHT] = RANK_1 & (FILE_B | FILE_G);
	pBoard->_pieces[BLACK][KNIGHT] = RANK_8 & (FILE_B | FILE_G);

	pBoard->_pieces[WHITE][BISHOP] = RANK_1 & (FILE_C | FILE_F);
	pBoard->_pieces[BLACK][BISHOP] = RANK_8 & (FILE_C | FILE_F);

	pBoard->_pieces[WHITE][QUEEN] = RANK_1 & (FILE_D);
	pBoard->_pieces[BLACK][QUEEN] = RANK_8 & (FILE_D);

	pBoard->_pieces[WHITE][KING] = RANK_1 & (FILE_E);
	pBoard->_pieces[BLACK][KING] = RANK_8 & (FILE_E);

	pBoard->_allPieces[WHITE] = pBoard->_pieces[WHITE][PAWN] | pBoard->_pieces[WHITE][ROOK] | pBoard->_pieces[WHITE][KNIGHT] | pBoard->_pieces[WHITE][BISHOP] | pBoard->_pieces[WHITE][QUEEN]
			| pBoard->_pieces[WHITE][KING];
	pBoard->_allPieces[BLACK] = pBoard->_pieces[BLACK][PAWN] | pBoard->_pieces[BLACK][ROOK] | pBoard->_pieces[BLACK][KNIGHT] | pBoard->_pieces[BLACK][BISHOP] | pBoard->_pieces[BLACK][QUEEN]
			| pBoard->_pieces[BLACK][KING];

	pBoard->_occupied = pBoard->_allPieces[WHITE] | pBoard->_allPieces[BLACK];
	pBoard->_notOccupied = ~pBoard->_occupied;

	pBoard->_ActivePlayer = WHITE;

	pBoard->_castlingRights = CASTLING_WHITE_QUEEN | CASTLING_WHITE_KING | CASTLING_BLACK_QUEEN | CASTLING_BLACK_KING;
	pBoard->_enPassant = ZERO;

	pBoard->_bestMove._move = 0;
	pBoard->_bestMove._value = 0;
}

void boardInitFen(sboard * pBoard, char* pFEN) {
	char tmp[512];
	sprintf(tmp, "%s", pFEN);
	char* token = strtok(tmp, " ");

	boardClear(pBoard);
	int idx = 0;
	U64 boardPos = 56; // Fen string starts at a8 = index 56
	while (token[idx]) {
		switch (token[idx]) {
		case 'p':
			pBoard->_pieces[BLACK][PAWN] |= (ONE << boardPos++);
			break;
		case 'r':
			pBoard->_pieces[BLACK][ROOK] |= (ONE << boardPos++);
			break;
		case 'n':
			pBoard->_pieces[BLACK][KNIGHT] |= (ONE << boardPos++);
			break;
		case 'b':
			pBoard->_pieces[BLACK][BISHOP] |= (ONE << boardPos++);
			break;
		case 'q':
			pBoard->_pieces[BLACK][QUEEN] |= (ONE << boardPos++);
			break;
		case 'k':
			pBoard->_pieces[BLACK][KING] |= (ONE << boardPos++);
			break;
		case 'P':
			pBoard->_pieces[WHITE][PAWN] |= (ONE << boardPos++);
			break;
		case 'R':
			pBoard->_pieces[WHITE][ROOK] |= (ONE << boardPos++);
			break;
		case 'N':
			pBoard->_pieces[WHITE][KNIGHT] |= (ONE << boardPos++);
			break;
		case 'B':
			pBoard->_pieces[WHITE][BISHOP] |= (ONE << boardPos++);
			break;
		case 'Q':
			pBoard->_pieces[WHITE][QUEEN] |= (ONE << boardPos++);
			break;
		case 'K':
			pBoard->_pieces[WHITE][KING] |= (ONE << boardPos++);
			break;
		case '/':
			boardPos -= 16; // Go down one rank
			break;
		default:
			boardPos += token[idx] - '0';
		}
		idx++;
	}

// Next to move
	token = strtok(NULL, " ");
	pBoard->_ActivePlayer = token[0] == 'w' ? WHITE : BLACK;

// Castling
	token = strtok(NULL, " ");
	pBoard->_castlingRights = 0;
	idx = 0;
	while (token[idx]) {
		switch (token[idx]) {
		case 'Q':
			pBoard->_castlingRights |= CASTLING_WHITE_QUEEN;
			break;
		case 'K':
			pBoard->_castlingRights |= CASTLING_WHITE_KING;
			break;
		case 'q':
			pBoard->_castlingRights |= CASTLING_BLACK_QUEEN;
			break;
		case 'k':
			pBoard->_castlingRights |= CASTLING_BLACK_KING;
			break;
		}
		idx++;
	}

// En passant target square
	/*
	 fenStream >> token;
	 _enPassant = token == "-" ? ZERO : ONE << Move
	 ::notationToIndex(token);
	 */

	pBoard->_allPieces[WHITE] = pBoard->_pieces[WHITE][PAWN] | pBoard->_pieces[WHITE][ROOK] | pBoard->_pieces[WHITE][KNIGHT] | pBoard->_pieces[WHITE][BISHOP] | pBoard->_pieces[WHITE][QUEEN]
			| pBoard->_pieces[WHITE][KING];
	pBoard->_allPieces[BLACK] = pBoard->_pieces[BLACK][PAWN] | pBoard->_pieces[BLACK][ROOK] | pBoard->_pieces[BLACK][KNIGHT] | pBoard->_pieces[BLACK][BISHOP] | pBoard->_pieces[BLACK][QUEEN]
			| pBoard->_pieces[BLACK][KING];

	pBoard->_occupied = pBoard->_allPieces[WHITE] | pBoard->_allPieces[BLACK];
	pBoard->_notOccupied = ~pBoard->_occupied;

}

void boardCpy(sboard * dst, sboard * src) {
	memcpy(dst, src, sizeof(sboard));
}

void _removePiece(sboard * pBoard, Color color, PieceType pieceType, int squareIndex) {
	U64 square = ONE << squareIndex;

	pBoard->_pieces[color][pieceType] ^= square;
	pBoard->_allPieces[color] ^= square;

	pBoard->_occupied ^= square;
	pBoard->_notOccupied = ~pBoard->_occupied;

	if (pieceType != KING) {
		pBoard->_attackable[color] ^= square;
	}

}

void _addPiece(sboard * pBoard, Color color, PieceType pieceType, int squareIndex) {
	U64 square = ONE << squareIndex;

	pBoard->_pieces[color][pieceType] |= square;
	pBoard->_allPieces[color] |= square;

	pBoard->_occupied |= square;
	pBoard->_notOccupied = ~pBoard->_occupied;

	if (pieceType != KING) {
		pBoard->_attackable[color] ^= square;
	}
}

void _movePiece(sboard * pBoard, Color color, PieceType pieceType, int from, int to) {
	_removePiece(pBoard, color, pieceType, from);
	_addPiece(pBoard, color, pieceType, to);
}

void doMove(sboard * pBoard, smove* move) {
	unsigned int flags = MOVE_FLAG(move->_move);

// En passant always cleared after a move
	pBoard->_enPassant = ZERO;

// Handle move depending on what type of move it is
	if (!flags) {
// No flags set, not a special move
		_movePiece(pBoard, pBoard->_ActivePlayer, MOVE_PIECE(move->_move), MOVE_FROM(move->_move), MOVE_TO(move->_move));
	} else if ((flags & CAPTURE) && (flags & PROMOTION)) { // Capture promotion special case
// Remove captured Piece
		_removePiece(pBoard, !pBoard->_ActivePlayer, MOVE_PIECE_CAPTURED(move->_move), MOVE_TO(move->_move));

// Remove promoting pawn
		_removePiece(pBoard, pBoard->_ActivePlayer, PAWN, MOVE_FROM(move->_move));

// Add promoted piece
		_addPiece(pBoard, pBoard->_ActivePlayer, MOVE_PIECE_PROMOTION(move->_move), MOVE_TO(move->_move));
	} else if (flags & CAPTURE) {
// Remove captured Piece
		_removePiece(pBoard, !pBoard->_ActivePlayer, MOVE_PIECE_CAPTURED(move->_move), MOVE_TO(move->_move));

// Move capturing piece
		_movePiece(pBoard, pBoard->_ActivePlayer, MOVE_PIECE(move->_move), MOVE_FROM(move->_move), MOVE_TO(move->_move));
	} else if (flags & KSIDE_CASTLE) {
		// Move the king
		_movePiece(pBoard, pBoard->_ActivePlayer, KING, MOVE_FROM(move->_move), MOVE_TO(move->_move));

		// Move the correct rook
		if (pBoard->_ActivePlayer == WHITE) {
			_movePiece(pBoard, WHITE, ROOK, h1, f1);
		} else {
			_movePiece(pBoard, BLACK, ROOK, h8, f8);
		}

	} else if (flags & QSIDE_CASTLE) {
		// Move the king
		_movePiece(pBoard, pBoard->_ActivePlayer, KING, MOVE_FROM(move->_move), MOVE_TO(move->_move));

		// Move the correct rook
		if (pBoard->_ActivePlayer == WHITE) {
			_movePiece(pBoard, WHITE, ROOK, a1, d1);
		} else {
			_movePiece(pBoard, BLACK, ROOK, a8, d8);
		}
	} else if (flags & EN_PASSANT) {

		// Remove the correct pawn
		if (pBoard->_ActivePlayer == WHITE) {
			_removePiece(pBoard, BLACK, PAWN, MOVE_TO(move->_move) - 8);
		} else {
			_removePiece(pBoard, WHITE, PAWN, MOVE_TO(move->_move) + 8);
		}

		// Move the capturing pawn
		_movePiece(pBoard, pBoard->_ActivePlayer, MOVE_PIECE(move->_move), MOVE_FROM(move->_move), MOVE_TO(move->_move));

	} else if (flags & PROMOTION) {
// Remove promoted pawn
		_removePiece(pBoard, pBoard->_ActivePlayer, PAWN, MOVE_FROM(move->_move));

// Add promoted piece
		_addPiece(pBoard, pBoard->_ActivePlayer, MOVE_PIECE_PROMOTION(move->_move), MOVE_TO(move->_move));

	} else if (flags & DOUBLE_PAWN_PUSH) {

		_movePiece(pBoard, pBoard->_ActivePlayer, MOVE_PIECE(move->_move), MOVE_FROM(move->_move), MOVE_TO(move->_move));
		// Set square behind pawn as _enPassant
		unsigned int enPasIndex = pBoard->_ActivePlayer == WHITE ?
		MOVE_TO(move->_move) - 8 :
																	MOVE_TO(move->_move) + 8;
		pBoard->_enPassant = ONE << enPasIndex;

	}
	/* FIXME TO BE IMPLEMENTED
	 // Halfmove clock reset on pawn moves or captures, incremented otherwise
	 if (move.getPieceType() == PAWN || move.getFlags() & Move::CAPTURE) {
	 _halfmoveClock = 0;
	 } else {
	 _halfmoveClock++;
	 }*/

	_updateCastlingRightsForMove(pBoard, move);
	pBoard->_ActivePlayer = !pBoard->_ActivePlayer;
}

void _updateCastlingRightsForMove(sboard * pBoard, smove* move) {
	unsigned int flags = MOVE_FLAG(move->_move);

	// Update castling flags if rooks have been captured
	if (flags & CAPTURE) {
		// Update castling rights if a rook was captured
		switch (MOVE_TO(move->_move)) {
		case a1:
			pBoard->_castlingRights &= ~CASTLING_WHITE_QUEEN;
			break;
		case h1:
			pBoard->_castlingRights &= ~CASTLING_WHITE_KING;
			break;
		case a8:
			pBoard->_castlingRights &= ~CASTLING_BLACK_QUEEN;
			break;
		case h8:
			pBoard->_castlingRights &= ~CASTLING_BLACK_KING;
			break;
		}
	}

	// Update castling flags if rooks or kings have moved
	switch (MOVE_FROM(move->_move)) {
	case e1:
		pBoard->_castlingRights &= ~(CASTLING_WHITE_QUEEN | CASTLING_WHITE_KING);
		break;
	case e8:
		pBoard->_castlingRights &= ~(CASTLING_BLACK_QUEEN | CASTLING_BLACK_KING);
		break;
	case a1:
		pBoard->_castlingRights &= ~CASTLING_WHITE_QUEEN;
		break;
	case h1:
		pBoard->_castlingRights &= ~CASTLING_WHITE_KING;
		break;
	case a8:
		pBoard->_castlingRights &= ~CASTLING_BLACK_QUEEN;
		break;
	case h8:
		pBoard->_castlingRights &= ~CASTLING_BLACK_KING;
		break;
	}

}

void boardGenerateAllMoves(sboard* board, smoveList* moveList) {
	PieceType ptype = 0;

	while (ptype < 6) {
		U64 ptm = board->_pieces[board->_ActivePlayer][ptype];
		while (ptm) {
			int from = _popLsb(&ptm);
			getMovesForSquare(board, moveList, ptype, board->_ActivePlayer, from);
		}
		ptype++;
	}

	if (board->_ActivePlayer == WHITE) {
		if (whiteCanCastleKs(board)) {
			moveBuildCastle(&moveList->_sMoveList[moveList->_nbrMove++], e1, g1, KSIDE_CASTLE);
		}
		if (whiteCanCastleQs(board)) {
			moveBuildCastle(&moveList->_sMoveList[moveList->_nbrMove++], e1, c1, QSIDE_CASTLE);
		}
	}

	if (board->_ActivePlayer == BLACK) {
		if (blackCanCastleKs(board)) {
			moveBuildCastle(&moveList->_sMoveList[moveList->_nbrMove++], e8, g8, KSIDE_CASTLE);
		}
		if (blackCanCastleQs(board)) {
			moveBuildCastle(&moveList->_sMoveList[moveList->_nbrMove++], e8, c8, QSIDE_CASTLE);
		}
	}

}

void boardGenerateAllLegalMoves(sboard* board, smoveList* moveList) {
	smoveList tmpList;
	sboard tmpBoard;
	moveListInit(&tmpList);
	moveListInit(moveList);

	boardGenerateAllMoves(board, &tmpList);

	for (int ii = 0; ii < tmpList._nbrMove; ii++) {
		boardCpy(&tmpBoard, board);
		doMove(&tmpBoard, &tmpList._sMoveList[ii]);
		if (!colorIsInCheck(&tmpBoard, !tmpBoard._ActivePlayer)) {
			{
				moveCpy(&moveList->_sMoveList[moveList->_nbrMove], &tmpList._sMoveList[ii]);
				moveList->_nbrMove++;
			}
		}
	}
}

void boardAddMovesPromotion(sboard* board, smoveList* moveList, int from, PieceType pieceType, U64 moves, U64 attackable) {
// Ignore all moves/attacks to kings
	moves &= ~(board->_pieces[!board->_ActivePlayer][KING]);

// Generate non attacks
	U64 nonAttacks = moves & ~attackable;
	while (nonAttacks) {
		int to = _popLsb(&nonAttacks);

		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], from, to, PAWN, 0, QUEEN);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], from, to, PAWN, 0, ROOK);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], from, to, PAWN, 0, BISHOP);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], from, to, PAWN, 0, KNIGHT);
	}

// Generate attacks
	U64 attacks = moves & attackable;
	while (attacks) {
		int to = _popLsb(&attacks);

		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], from, to, PAWN, getPieceAtSquare(board, !board->_ActivePlayer, to), QUEEN);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], from, to, PAWN, getPieceAtSquare(board, !board->_ActivePlayer, to), ROOK);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], from, to, PAWN, getPieceAtSquare(board, !board->_ActivePlayer, to), BISHOP);
		moveBuildPromotion(&moveList->_sMoveList[moveList->_nbrMove++], from, to, PAWN, getPieceAtSquare(board, !board->_ActivePlayer, to), KNIGHT);

	}
}

void boardAddMovesEnPassant(sboard* board, smoveList* moveList, int from, PieceType pieceType, U64 moves) {
// Ignore all moves/attacks to kings
	moves &= ~(board->_pieces[!board->_ActivePlayer][KING]);

// Generate non attacks
	U64 nonAttacks = moves;
	while (nonAttacks) {
		int to = _popLsb(&nonAttacks);

		moveBuildEnPassant(&moveList->_sMoveList[moveList->_nbrMove++], from, to, pieceType);
	}

}

void boardAddMoves(sboard* board, smoveList* moveList, int from, PieceType pieceType, U64 moves, U64 attackable) {
// Ignore all moves/attacks to kings
	moves &= ~(board->_pieces[!board->_ActivePlayer][KING]);

// Generate non attacks
	U64 nonAttacks = moves & ~attackable;
	while (nonAttacks) {
		int to = _popLsb(&nonAttacks);
		moveBuild(&moveList->_sMoveList[moveList->_nbrMove++], from, to, pieceType);
	}

// Generate attacks
	U64 attacks = moves & attackable;
	while (attacks) {
		int to = _popLsb(&attacks);
		moveBuildCapture(&moveList->_sMoveList[moveList->_nbrMove++], from, to, pieceType, getPieceAtSquare(board, !board->_ActivePlayer, to));
	}
}

U64 getMovesForSquare(sboard * pBoard, smoveList* moveList, PieceType pieceType, Color color, int square) {
// Special case for pawns
	if (pieceType == PAWN) {
		switch (color) {
		case WHITE:
			getWhitePawnAttacks(pBoard, square, moveList);
			getWhitePawnMove(pBoard, square, moveList);
			break;
		case BLACK:
			getBlackPawnAttacks(pBoard, square, moveList);
			getBlackPawnMove(pBoard, square, moveList);
			break;
		}
		return 0;
	}

	U64 own = pBoard->_allPieces[color];

	U64 attacks = ZERO;
	switch (pieceType) {
	case ROOK:
		attacks = getRookAttacks(square, pBoard->_occupied) & ~pBoard->_allPieces[color];
		boardAddMoves(pBoard, moveList, square, pieceType, attacks, pBoard->_allPieces[!color]);
		break;
	case KNIGHT:
		attacks = getKnightAttacks(square) & ~pBoard->_allPieces[color];
		boardAddMoves(pBoard, moveList, square, pieceType, attacks, pBoard->_allPieces[!color]);
		break;
	case BISHOP:
		attacks = getBishopAttacks(square, pBoard->_occupied) & ~pBoard->_allPieces[color];
		boardAddMoves(pBoard, moveList, square, pieceType, attacks, pBoard->_allPieces[!color]);
		break;
	case QUEEN:
		attacks = getQueenAttacks(square, pBoard->_occupied) & ~pBoard->_allPieces[color];
		boardAddMoves(pBoard, moveList, square, pieceType, attacks, pBoard->_allPieces[!color]);
		break;
	case KING:
		attacks = getKingAttacks(square) & ~pBoard->_allPieces[color];
		boardAddMoves(pBoard, moveList, square, pieceType, attacks, pBoard->_allPieces[!color]);
		break;

	case PAWN:
		printf("Error getAttacksForSquare , should never be reach\n");
		break;
	}

	return attacks & ~own;
}

PieceType getPieceAtSquare(sboard * pBoard, Color color, int squareIndex) {

	U64 square = ONE << squareIndex;

	PieceType piece = 0;

	if (square & pBoard->_pieces[color][PAWN])
		piece = PAWN;
	else if (square & pBoard->_pieces[color][ROOK])
		piece = ROOK;
	else if (square & pBoard->_pieces[color][KNIGHT])
		piece = KNIGHT;
	else if (square & pBoard->_pieces[color][BISHOP])
		piece = BISHOP;
	else if (square & pBoard->_pieces[color][KING])
		piece = KING;
	else if (square & pBoard->_pieces[color][QUEEN])
		piece = QUEEN;
	else
		printf("getPieceAtSquare should be never reach(square index:%i)\n", squareIndex);

	return piece;
}

void boardPrintMove(U64 m) {
	printf(" ABCDEFGH\n");
	for (int jj = 56; jj >= 0; jj -= 8) {
		printf("%i", 1 + (jj / 8));
		for (int ii = 0; ii < 8; ii++) {
			if (m >> (ii + jj) & ONE)
				printf(".");
			else
				printf(" ");
		}
		printf("\n");
	}
	printf(" ABCDEFGH\n");
}

void boardPrint(sboard * pBoard) {
	printf(" ABCDEFGH\n");
	for (int jj = 56; jj >= 0; jj -= 8) {
		printf("%i", 1 + (jj / 8));
		for (int ii = 0; ii < 8; ii++) {
			if (pBoard->_occupied >> (ii + jj) & ONE) {
				if (pBoard->_pieces[WHITE][PAWN] >> (ii + jj) & ONE)
					printf("P");
				if (pBoard->_pieces[BLACK][PAWN] >> (ii + jj) & ONE)
					printf("p");

				if (pBoard->_pieces[WHITE][ROOK] >> (ii + jj) & ONE)
					printf("R");
				if (pBoard->_pieces[BLACK][ROOK] >> (ii + jj) & ONE)
					printf("r");

				if (pBoard->_pieces[WHITE][KNIGHT] >> (ii + jj) & ONE)
					printf("N");
				if (pBoard->_pieces[BLACK][KNIGHT] >> (ii + jj) & ONE)
					printf("n");

				if (pBoard->_pieces[WHITE][BISHOP] >> (ii + jj) & ONE)
					printf("B");
				if (pBoard->_pieces[BLACK][BISHOP] >> (ii + jj) & ONE)
					printf("b");

				if (pBoard->_pieces[WHITE][QUEEN] >> (ii + jj) & ONE)
					printf("Q");
				if (pBoard->_pieces[BLACK][QUEEN] >> (ii + jj) & ONE)
					printf("q");

				if (pBoard->_pieces[WHITE][KING] >> (ii + jj) & ONE)
					printf("K");
				if (pBoard->_pieces[BLACK][KING] >> (ii + jj) & ONE)
					printf("k");
			} else
				printf(" ");
		}
		printf("\n");
	}
	printf(" ABCDEFGH\n\n");
}

int whiteCanCastleKs(sboard * pBoard) {
	if (!(pBoard->_castlingRights & CASTLING_WHITE_KING)) {
		return 0;
	}

	U64 passThroughSquares = (ONE << f1) | (ONE << g1);
	U64 squaresOccupied = passThroughSquares & pBoard->_occupied;
	U64 squaresAttacked = _squareUnderAttack(pBoard, BLACK, f1) || _squareUnderAttack(pBoard, BLACK, g1);

	return !colorIsInCheck(pBoard, WHITE) && !squaresOccupied && !squaresAttacked;
}

int whiteCanCastleQs(sboard * pBoard) {
	if (!(pBoard->_castlingRights & CASTLING_WHITE_QUEEN)) {
		return 0;
	}

	U64 inbetweenSquares = (ONE << c1) | (ONE << d1) | (ONE << b1);
	U64 squaresOccupied = inbetweenSquares & pBoard->_occupied;
	U64 squaresAttacked = _squareUnderAttack(pBoard, BLACK, d1) || _squareUnderAttack(pBoard, BLACK, c1);

	return !colorIsInCheck(pBoard, WHITE) && !squaresOccupied && !squaresAttacked;
}

int blackCanCastleKs(sboard * pBoard) {
	if (!(pBoard->_castlingRights & CASTLING_BLACK_KING)) {
		return 0;
	}

	U64 passThroughSquares = (ONE << f8) | (ONE << g8);
	U64 squaresOccupied = passThroughSquares & pBoard->_occupied;
	U64 squaresAttacked = _squareUnderAttack(pBoard, WHITE, f8) || _squareUnderAttack(pBoard, WHITE, g8);

	return !colorIsInCheck(pBoard, BLACK) && !squaresOccupied && !squaresAttacked;
}

int blackCanCastleQs(sboard * pBoard) {
	if (!(pBoard->_castlingRights & CASTLING_BLACK_QUEEN)) {
		return 0;
	}

	U64 inbetweenSquares = (ONE << b8) | (ONE << c8) | (ONE << d8);
	U64 squaresOccupied = inbetweenSquares & pBoard->_occupied;
	U64 squaresAttacked = _squareUnderAttack(pBoard, WHITE, c8) || _squareUnderAttack(pBoard, WHITE, d8);

	return !colorIsInCheck(pBoard, BLACK) && !squaresOccupied && !squaresAttacked;
}

int _squareUnderAttack(sboard * pBoard, Color color, int squareIndex) {
// Check for pawn, knight and king attacks
	if (getNonSlidingAttacks(PAWN, squareIndex, !color) & pBoard->_pieces[color][PAWN])
		return 1;
	if (getNonSlidingAttacks(KNIGHT, squareIndex, !color) & pBoard->_pieces[color][KNIGHT])
		return 1;
	if (getNonSlidingAttacks(KING, squareIndex, !color) & pBoard->_pieces[color][KING])
		return 1;

// Check for bishop/queen attacks
	U64 bishopsQueens = pBoard->_pieces[color][BISHOP] | pBoard->_pieces[color][QUEEN];
	if (getBishopAttacks(squareIndex, pBoard->_occupied) & bishopsQueens)
		return 1;

// Check for rook/queen attacks
	U64 rooksQueens = pBoard->_pieces[color][ROOK] | pBoard->_pieces[color][QUEEN];
	if (getRookAttacks(squareIndex, pBoard->_occupied) & rooksQueens)
		return 1;

	return 0;
}

int colorIsInCheck(sboard * pBoard, Color color) {
	int kingSquare = _bitscanForward(pBoard->_pieces[color][KING]);
	return _squareUnderAttack(pBoard, !color, kingSquare);
}
