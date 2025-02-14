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

int perft(sboard* pBoard, int* pCnt, int depth, int iteration);
int perftRun(char* posStart, int depth, int expected);
int perftMasterRun(char* posStart, int e1, int e2, int e3, int e4, int e5, int e6);


int perftCheckFile(char* fileName, int depth);
int puzzlzCheckFile(char* fileName, int depth);
int benchCheckFile(char* fileName, int depth);

int puzzleMasterRun(char* posStart, int depth, int* nbrNode, int* nbrCut, int* nbrZob, int* nbrQuies);
int benchMasterRun(char* posStart, int depth, int* nbrNode, int* nbrCut, int* nbrZob, int* nbrQuies);

#endif /* SRC_PERFT_H_ */
