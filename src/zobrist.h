/*
 * zobrist .h
 *
 *  Created on: 25 août 2018
 *      Author: Bertrand
 */

#ifndef SRC_ZOBRIST_H_
#define SRC_ZOBRIST_H_

#include "defs.h"

void zobInit(void);


/*
 * zobrist .c
 *
 *  Created on: 25 août 2018
 *      Author: Bertrand
 */
#include <stdlib.h>
#include "board.h"
#include "zobrist.h"
#include "bitutils.h"

U64 PIECE_KEYS[2][6][64];
U64 EN_PASSANT_KEYS[8];
U64 KS_CASTLE_KEYS[2];
U64 QS_CASTLE_KEYS[2];
U64 WHITE_TO_MOVE_KEY;


U64 zobFlipPiece(Color color, PieceType piece, unsigned int index, U64 key);
U64 zobFlipKsCastle(Color color, U64 key);
U64 zobFlipQsCastle(Color color, U64 key);
U64 zobFlipPLayer(U64 key);
U64 zobEnPassant(U64 key, unsigned int file);

U64 zobCompute(sboard* board);
#endif /* SRC_ZOBRIST_H_ */
