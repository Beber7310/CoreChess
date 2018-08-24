/*
 * search.h
 *
 *  Created on: 21 août 2018
 *      Author: Bertrand
 */

#ifndef SRC_SEARCH_H_
#define SRC_SEARCH_H_


smove searchStart(sboard * pBoard,int depth);
int negamax(sboard * pNode, int depth, int alpha, int beta, Color color);



#endif /* SRC_SEARCH_H_ */
