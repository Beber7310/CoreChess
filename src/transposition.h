/*
 * transposition.h
 *
 *  Created on: Aug 29, 2018
 *      Author: dosdab
 */

#ifndef SRC_TRANSPOSITION_H_
#define SRC_TRANSPOSITION_H_

#include "board.h"

typedef enum {
	 EMPTY=0,EXACT,LOWERBOUND,UPPERBOUND
}ttFlag;

typedef struct ttEntry_t {
	sboard	board;
	ttFlag flag;
	int value;
	int depth;
}ttEntry;

ttEntry* ttGet(U64 key);
void ttSet(sboard *pBoard, int value, int depth, ttFlag flag);

#endif /* SRC_TRANSPOSITION_H_ */
