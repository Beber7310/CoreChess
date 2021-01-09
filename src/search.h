/*
 * search.h
 *
 *  Created on: 21 août 2018
 *      Author: Bertrand
 */

#ifndef SRC_SEARCH_H_
#define SRC_SEARCH_H_

#include <time.h>

#define INF (99999)



typedef struct searchStat_t {
	time_t startSearchTIme;
	int maxSearchTime;
	int maxDepth;
	int nbrNode;
	int nbrCut;
	int nbrZob;
	int nbrQuies;
	int boardEval;
}searchStat;


typedef enum search_state_t
{
	SEARCH_FIRST,
	SEARCH_ALPHA,
	SEARCH_QUIESSENCE
}search_state;

typedef int (*AlgoFunc) (sboard* pNode, int depth, Color color, int alpha, int beta, searchStat* stat);

void searchCheckTime(searchStat* stat);
smove searchStart(sboard * pBoard, int wtime, int btime, int mtime, int moveToGo, searchStat* stat);

#endif /* SRC_SEARCH_H_ */
