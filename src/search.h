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
	int boardEval;
	smoveList pv;

}searchStat;



smove searchStart(sboard * pBoard, int wtime, int btime, int moveToGo, searchStat* stat);
int negamax(sboard * pNode, int depth, int alpha, int beta, Color color, searchStat* stat);
int negamaxTT(sboard * pNode, int depth, int alpha, int beta, Color color, searchStat* stat);

#endif /* SRC_SEARCH_H_ */
