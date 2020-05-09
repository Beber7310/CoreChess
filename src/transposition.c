/*
 * transposition.c
 *
 *  Created on: Aug 29, 2018
 *      Author: dosdab
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "transposition.h"

static ttEntry* ttTable;
static int gSize;
static int gMaskTT;

int ttInit(int size) {
	ttTable = malloc(size);
	if (ttTable == NULL) {
		printf("Error while malloc ttInit\n");
	}
	memset(ttTable, 0, size);

	if (sizeof(ttEntry) > 16) {
		printf("ERROR: ttEntry to big:%i!!!", sizeof(ttEntry));
		exit(-1);
	}
	gMaskTT = (size - 1) >> 4;

	gSize = size;
	return 0;
}
 
ttEntry* ttGet(U64 key) {
	ttEntry* tt;
	tt = &ttTable[key & gMaskTT];
	if (key == tt->key) {
		return tt;
	}
	return NULL;
}

ttEntry* ttAllocate(U64 key) {
	ttEntry* tt;
	tt = &ttTable[key & gMaskTT];

	return tt;

}

void ttSet(sboard *pBoard, int32_t value, uint8_t depth, ttFlag flag) {
	ttEntry* prev;
	
	int writeNew = 1;

	prev = ttGet(pBoard->_zobKey);

	
	if (prev == NULL) {
		writeNew = 1;
		prev = ttAllocate(pBoard->_zobKey);
	}else if (prev->flag == EMPTY) {
		writeNew = 1;
	} else if (depth > prev->depth) {
		writeNew = 1;
	} else if ((depth == prev->depth) && (prev->flag == EXACT)) {
		writeNew = 1;
	}
	
	/*
	if (writeNew) {
		prev->key = pBoard->_zobKey;
		prev->depth = depth;
		prev->flag = flag;
		prev->value = value;
	}
	*/
}
