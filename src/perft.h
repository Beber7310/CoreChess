/*
 * perft.h
 *
 *  Created on: 15 août 2018
 *      Author: Bertrand
 */

#ifndef SRC_PERFT_H_
#define SRC_PERFT_H_

#include "board.h"
#include "movegen.h"

void perft(sboard* pBoard, int* pCnt, int depth);
void perftCheck(void);
void perftRun(char* posStart, int depth, int expected);

#endif /* SRC_PERFT_H_ */
