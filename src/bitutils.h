/*
 * bitutils.h
 *
 *  Created on: 9 août 2018
 *      Author: Bertrand
 */

#ifndef SRC_BITUTILS_H_
#define SRC_BITUTILS_H_

/*
 * bitutils.c
 *
 *  Created on: 9 août 2018
 *      Author: Bertrand
 */
#ifdef _MSC_VER
    //#pragma intrinsic(_BitScanReverse)

    #define max(a,b) (((a) > (b)) ? (a) : (b))
    #define min(a,b) (((a) < (b)) ? (a) : (b))
#else
   int max(int a, int b);
   int min(int a, int b);
#endif

 
#include "defs.h"

/**
 * @brief Sets the LSB of the given bitboard to 0 and returns its index.
 *
 * @param  board Value to reset LSB of
 * @return Index of reset LSB
 */
int _popLsb(U64* board);

/**
 * @brief Returns the number of set bits in the given value.
 *
 * @param  board Value to return number of set bits for
 * @return Number of set bits in value
 */
int _popCount(U64 board);

/**
 * @brief Returns the index of the LSB in the given bitboard.
 *
 * @param  board Bitboard to get LSB of
 * @return The index of the LSB in the given bitboard.
 */
int _bitscanForward(U64 board);

/**
 * @brief Returns the index of the MSB in the given bitboard.
 *
 * @param  board Bitboard to get MSB of
 * @return The index of the MSB in the given bitboard.
 */
int _bitscanReverse(U64 board);

/**
 * @brief Moves all set bits in the given bitboard one square east and returns the new bitboard, discarding those that fall off the edge.
 * @param  board Board to move bits east on.
 * @return A bitboard with all set bits moved one square east, bits falling off the edge discarded
 */
U64 _eastOne(U64 board);

/**
 * @brief Moves all set bits in the given bitboard one square west and returns the new bitboard, discarding those that fall off the edge.
 * @param  board Board to move bits west on.
 * @return A bitboard with all set bits moved one square west, bits falling off the edge discarded
 */
U64 _westOne(U64 board);



#endif /* SRC_BITUTILS_H_ */
