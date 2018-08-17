/*
 * move.h
 *
 *  Created on: 12 ao�t 2018
 *      Author: Bertrand
 */

#ifndef SRC_MOVE_H_
#define SRC_MOVE_H_

/**
 * @enum Flag
 * @brief Flags that indicate special moves.
 */
enum Flag {
	NULL_MOVE = 1 << 0, /**< The move is a null move */
	CAPTURE = 1 << 1, /**< The move is a capture (Captured piece type must be set with setCapturedPieceType()) */
	DOUBLE_PAWN_PUSH = 1 << 2, /**< The move is a double pawn push */
	KSIDE_CASTLE = 1 << 3, /**< The move is a kingisde castle */
	QSIDE_CASTLE = 1 << 4, /**< The move is a queenside castle */
	EN_PASSANT = 1 << 5, /**< The move is an en passant capture (Do not set the CAPTURE flag additionally) */
	PROMOTION = 1 << 6 /**< The move is a promotion (Promotion piece type must be set with setPromotionPieceType()) */
};

#define MOVE_FROM(x) ((x>>9)&0x3F)
#define MOVE_TO(x) ((x>>15)&0x3F)
#define MOVE_FLAG(x) ((x>>21)&0x7F)
#define MOVE_PIECE(x) ((x>>0)&0x7)
#define MOVE_PIECE_CAPTURED(x) ((x>>6)&0x7)
#define MOVE_PIECE_PROMOTION(x) ((x>>3)&0x7)

typedef struct smove_t {
	/**
	 * @brief A packed integer containing all of the move data.
	 *
	 * 28 bits are used in total to store move information. The format is as
	 * follows:
	 *
	 * @code
	 * MSB  |----6----|---5---|---4---|-3--|-2--|-1--|  LSB
	 *      28        21      15      9    6    3    0
	 * @endcode
	 *
	 * - 6 - Flags (7 bits)
	 * - 5 - To square (6 bits)
	 * - 4 - From square (6 bits)
	 * - 3 - Captured PieceType (if applicable) (3 bits)
	 * - 2 - Promotion PieceType (if applicable) (3 bits)
	 * - 1 - PieceType of moving piece (3 bits)
	 *
	 * Moves are stored as a packed integer consisting of 28 bits total.
	 */
	unsigned int _move;

	/** @brief Value of this move */
	int _value;
} smove;

typedef struct smoveList_t {
	smove _sMoveList[256];
	int _nbrMove;
} smoveList;

void moveBuild(smove* pMove, unsigned int from, unsigned int to, PieceType piece);
void moveBuildCapture(smove* pMove, unsigned int from, unsigned int to, PieceType piece, PieceType pieceKilled);
void moveBuildPromotion(smove* pMove, unsigned int from, unsigned int to, PieceType piece, PieceType pieceKilled, PieceType pieceProm);
void moveBuildCastle(smove* pMove, unsigned int from, unsigned int to,  int flags);
void moveBuildDoublePawn(smove* pMove, unsigned int from, unsigned int to, PieceType piece);
void moveListInit(smoveList* pList);
void movePrint(smove* move);

#endif /* SRC_MOVE_H_ */
