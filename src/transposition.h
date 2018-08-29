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
	 EXACT,LOWERBOUND,UPPERBOUND
}ttFlag;

typedef struct ttEntry_t {
	sboard	board;
	ttFlag flag;
	int value;
	int depth;
}ttEntry;


#endif /* SRC_TRANSPOSITION_H_ */
