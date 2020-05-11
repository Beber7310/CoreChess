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
	//smoveList movelist;

}searchStat;

typedef int (*AlgoFunc) (sboard* pNode, int depth, Color color, int alpha, int beta, searchStat* stat);

void searchCheckTime(searchStat* stat);
smove searchStart(sboard * pBoard, int wtime, int btime, int moveToGo, searchStat* stat);
int negamax(sboard * pNode, int depth,  Color color, int alpha, int beta,searchStat* stat);
int alphaBeta(sboard* pNode, int depth,Color color,int alpha,int beta, searchStat* stat);
int negascout (sboard* pNode, int depth, Color color, int alpha, int beta, searchStat* stat);
smove algoCheck(sboard* pBoard,AlgoFunc algo1,AlgoFunc algo2, searchStat* stat);

#endif /* SRC_SEARCH_H_ */
