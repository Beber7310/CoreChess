/*
 * bitutils.c
 *
 *  Created on: 9 août 2018
 *      Author: Bertrand
 */

#include "defs.h"



#ifdef _MSC_VER
    #include<intrin.h>


int _popCount(U64 board) {
    int res;
    res= (int)__popcnt64(board);
    return res;
   
}

/**
 * @brief Sets the LSB of the given bitboard to 0 and returns its index.
 *
 * @param  board Value to reset LSB of
 * @return Index of reset LSB
 */
int _popLsb(U64* board) {
    if (*board == 0)
        *board = 0;

    unsigned long lsbIndex=0;
    _BitScanForward64(&lsbIndex, *board);
    (*board) &= (*board - 1);
    return lsbIndex;
}

/**
* @brief Returns the index of the LSB in the given bitboard.
*
* @param  board Bitboard to get LSB of
* @return The index of the LSB in the given bitboard.
*/
int _bitscanForward(U64 board) {
    unsigned long res=0;

    _BitScanForward64(&res,board);
    return res;
}

/**
 * @brief Returns the index of the MSB in the given bitboard.
 *
 * @param  board Bitboard to get MSB of
 * @return The index of the MSB in the given bitboard.
 */
int _bitscanReverse(U64 board) {
    unsigned long res=0;
       
    _BitScanReverse64(&res, board);
    return 64-res;
}




#endif

/**
 * @brief Moves all set bits in the given bitboard one square east and returns the new bitboard, discarding those that fall off the edge.
 * @param  board Board to move bits east on.
 * @return A bitboard with all set bits moved one square east, bits falling off the edge discarded
 */
U64 _eastOne(U64 board) {
    return ((board << ONE) & (~FILE_A));
}

/**
 * @brief Moves all set bits in the given bitboard one square west and returns the new bitboard, discarding those that fall off the edge.
 * @param  board Board to move bits west on.
 * @return A bitboard with all set bits moved one square west, bits falling off the edge discarded
 */
U64 _westOne(U64 board) {
    return ((board >> ONE) & (~FILE_H));
}


#ifndef _MSC_VER
/**
 * @brief Sets the LSB of the given bitboard to 0 and returns its index.
 *
 * @param  board Value to reset LSB of
 * @return Index of reset LSB
 */
int _popLsb(U64* board) {
	int lsbIndex = __builtin_ffsll(*board) - 1;
	(*board) &= (*board-1);
	return lsbIndex;
}

/**
 * @brief Returns the number of set bits in the given value.
 *
 * @param  board Value to return number of set bits for
 * @return Number of set bits in value
 */
int _popCount(U64 board) {
	return __builtin_popcountll(board);
}

/**
 * @brief Returns the index of the LSB in the given bitboard.
 *
 * @param  board Bitboard to get LSB of
 * @return The index of the LSB in the given bitboard.
 */
int _bitscanForward(U64 board) {
	return __builtin_ffsll(board) - 1;
}

/**
 * @brief Returns the index of the MSB in the given bitboard.
 *
 * @param  board Bitboard to get MSB of
 * @return The index of the MSB in the given bitboard.
 */
int _bitscanReverse(U64 board) {
	return __builtin_clzll(board) + 1;
}



int max(int a, int b) {
	if (a > b)
		return a;
	else
		return b;
}

int min(int a, int b) {
	if (a < b)
		return a;
	else
		return b;
}

#else

#endif