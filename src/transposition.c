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

static ttEntry* ttTable = 0;
static int gSize;
static int gMaskTT;

static transpotitionStat stat;

int ttInit(int size) {
	if (ttTable)
		free(ttTable);

	size = size * 1024 * 1024;
	ttTable = malloc(size);
	if (ttTable == NULL) {
		printf("Error while malloc ttInit\n");
		return -1;
	}
	memset(ttTable, 0, size);

	if (sizeof(ttEntry) > 16) {
		printf("ERROR: ttEntry to big:%i!!!", (int)sizeof(ttEntry));
		exit(-1);
	}
	gMaskTT = (size - 1) >> 4;

	gSize = size;

	stat.reqHit = 0;
	stat.reqNbr = 0;
	stat.setMis = 0;
	stat.setHit = 0;

	return 0;
}

ttEntry* ttGet(U64 key) {
	ttEntry* tt;
	tt = &ttTable[key & gMaskTT];
	
	stat.reqNbr ++;
	if (key == tt->key) {
		stat.reqHit++;
		return tt;
	}
	return NULL;
}

ttEntry* ttAllocate(U64 key) {
	ttEntry* tt;
	tt = &ttTable[key & gMaskTT];

	return tt;

}

void ttSet(sboard* pBoard, int32_t value, uint8_t depth, ttFlag flag) {
	ttEntry* prev;

	int writeNew = 1;

	prev = ttGet(pBoard->_zobKey);
	 
	if (prev == NULL) {
		writeNew = 1;
		prev = ttAllocate(pBoard->_zobKey);
	}
	else if (prev->flag == EMPTY) {
		writeNew = 1;
	}
	else if (depth > prev->depth) {
		writeNew = 1;
	}
	else if ((depth == prev->depth) && (prev->flag == EXACT)) {
		writeNew = 1;
	}
	else
	{
		stat.setMis++;
	}

	if (writeNew) {
		stat.setHit++;
		prev->key = pBoard->_zobKey;
		prev->depth = depth;
		prev->flag = flag;
		prev->value = value;
	}

}

void ttPrintStat()
{
	char str[256];

	sprintf(str, "transpo stat: Nbr req %i req hit %i set hit %i set miss %i`\n", stat.reqNbr, stat.reqHit, stat.setHit, stat.setMis);
	printTcp(str);
}