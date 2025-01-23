/*
 * transposition.h
 *
 *  Created on: Aug 29, 2018
 *      Author: dosdab
 */

#ifndef SRC_TRANSPOSITION_H_
#define SRC_TRANSPOSITION_H_

#include "board.h"

void ttPrintStat();


typedef enum {
	 EMPTY=0,EXACT,LOWERBOUND,UPPERBOUND
}ttFlag;

typedef struct ttEntry_t {
	//sboard	board;
	U64 key;
	int32_t value;
	uint8_t depth;
	uint8_t flag;
}ttEntry;


typedef struct transpotitionStat_t {	
	int reqNbr;
	int reqHit;	
	int setMis;
	int setHit;
}transpotitionStat;


int ttInit(int size);
ttEntry* ttGet(U64 key);
void ttSet(sboard *pBoard, int32_t value, uint8_t depth, ttFlag flag);
void ttPrintStat();

#endif /* SRC_TRANSPOSITION_H_ */
