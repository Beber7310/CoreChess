/*
 * board.h
 *
 *  Created on: 7 ao�t 2018
 *      Author: Bertrand
 */

#ifndef SRC_BOARD_H_
#define SRC_BOARD_H_

#include <stdio.h>
#include "defs.h"
#include "move.h"

typedef struct sboard_t {
	/**
	 * @brief Array indexed by [color][piecetype] of piece bitboards
	 */
	U64 _pieces[2][6];

	/**
	 * @brief Array indexed by [color] of bitboards containing all pieces of each color.
	 */
	U64 _allPieces[2];

	/**
	 * @brief Array indexed by [color] of bitboards containing all attackable pieces.
	 */
	U64 _attackable[2];

	/**
	 * @brief Bitboard containing all occupied squares.
	 */
	U64 _occupied;

	/**
	 * @brief Bitboard containing all unoccupied squares.
	 */
	U64 _notOccupied;

	/**
	 * @brief
	 */
	Color _ActivePlayer;

	/**
	 * @brief
	 */
	U64 _castlingRights;

	U64 _enPassant;

	smove _bestMove;
} sboard;

void boardInit(sboard * pBoard);
void boardInitFen(sboard * pBoard, char* pFEN);

void boardPrintMove(U64 m);
U64 getMovesForSquare(sboard * pBoard, smoveList* moveList, PieceType pieceType, Color color, int square);
void boardGenerateAllMoves(sboard* board, smoveList* moveList);

void boardGenerateAllLegalMoves(sboard* board, smoveList* moveList);
void boardAddMoves(sboard* board, smoveList* moveList, int from, PieceType pieceType, U64 moves, U64 attackable);
void boardAddMovesPromotion(sboard* board, smoveList* moveList, int from, PieceType pieceType, U64 moves, U64 attackable);
void boardAddMovesEnPassant(sboard* board, smoveList* moveList, int from, PieceType pieceType, U64 moves);
PieceType getPieceAtSquare(sboard * pBoard, Color color, int squareIndex);

void _removePiece(sboard * pBoard, Color color, PieceType pieceType, int squareIndex);
void _addPiece(sboard * pBoard, Color color, PieceType pieceType, int squareIndex);
void _movePiece(sboard * pBoard, Color color, PieceType pieceType, int from, int to);
void boardCpy(sboard * dst, sboard * src);
void boardClear(sboard * pBoard);
void boardPrint(sboard * pBoard);

void doMove(sboard * pBoard, smove* move);
void _updateCastlingRightsForMove(sboard * pBoard,smove* move);

int colorIsInCheck(sboard * pBoard,Color color);
int _squareUnderAttack(sboard * pBoard, Color color, int squareIndex);
int blackCanCastleQs(sboard * pBoard);
int blackCanCastleKs(sboard * pBoard);
int whiteCanCastleQs(sboard * pBoard);
int whiteCanCastleKs(sboard * pBoard);
#endif /* SRC_BOARD_H_ */
