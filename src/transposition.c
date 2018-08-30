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
static int	gMaskTT;
static int	gShiftTT;

int ttInit(int size) {
	ttTable = malloc(size);
	if(ttTable==NULL)
	{
		printf("Error while malloc ttInit\n");
	}
	memset (ttTable,0,size);

	gMaskTT;
	gShiftTT;

	gSize=size;
	return 0;
}

ttEntry* ttGet(U64 key) {
	int idx=(key&gMaskTT)>>gShiftTT;
	return &ttTable[idx];
}

void ttSet(sboard *pBoard, int value, int depth, ttFlag flag) {

}
