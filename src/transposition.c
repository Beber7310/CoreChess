/*
 * transposition.c
 *
 *  Created on: Aug 29, 2018
 *      Author: dosdab
 */

#include <stdlib.h>
#include "transposition.h"


static ttEntry* ttTable;
static int gSize;
static U64	gMaskTT;
static U64	gShiftTT;

int ttInit(int size) {
	ttTable = malloc(size);
	gSize=size;

	return 0;
}

ttEntry* ttGet(U64 key) {
	int idx=(key>>gShiftTT)&gMaskTT;
	return &ttTable[idx];
}

void ttSet(sboard *pBoard, int value, int depth, ttFlag flag) {

}
