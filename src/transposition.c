/*
 * transposition.c
 *
 *  Created on: Aug 29, 2018
 *      Author: dosdab
 */

#include <stdlib.h>
#include "transposition.h"


static ttEntry* ttTable;

int ttInit(int size) {
	ttTable = malloc(size);
}

ttEntry* ttGet(U64 key) {

}

void ttSet(sboard *pBoard, int value, int depth, ttFlag flag) {

}
