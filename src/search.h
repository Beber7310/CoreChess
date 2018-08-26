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

smove searchStart(sboard * pBoard, int wtime, int btime, int moveToGo);
int negamax(sboard * pNode, int depth, int alpha, int beta, Color color);


typedef struct searchStat_t {
	time_t startSearchTIme;
	int maxSearchTime;
	int nbrNode;
	int nbrNodeCheck;

}searchStat;

#endif /* SRC_SEARCH_H_ */
