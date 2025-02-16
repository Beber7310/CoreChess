/*
 * search.h
 *
 *  Created on: 21 août 2018
 *      Author: Bertrand
 */

#ifndef SRC_SEARCH_H_
#define SRC_SEARCH_H_

#include <time.h>
#include "move.h"
#include <sys/timeb.h>


#define MAX_SEARCH_DEPTH 30
#define MAX_PV_DEPTH 6

typedef struct negaMaxConf_t {
	struct timeb  startSearchTIme;
	int maxSearchTime;
	int maxDepth;
	int nbrNode;
	int nbrCut;
	int nbrZob;
	int nbrQuies;
	int boardEval;
}negaMaxConf;


typedef enum search_state_t
{
	SEARCH_FIRST,
	SEARCH_ALPHA,
	SEARCH_QUIESSENCE
}search_state;

typedef int (*AlgoFunc) (sboard* pNode, int depth, Color color, int alpha, int beta, negaMaxConf* stat);

void searchCheckTime(negaMaxConf* stat);
smove searchStart(sboard* pBoard, int wtime, int btime, int winc, int binc,  int moveToGo, negaMaxConf* stat, smoveList* pastMoves);

#endif /* SRC_SEARCH_H_ */
