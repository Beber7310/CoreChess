/*
 * moveOrder.h
 *
 *  Created on: Aug 27, 2018
 *      Author: dosdab
 */

#ifndef SRC_MOVEORDER_H_
#define SRC_MOVEORDER_H_

void moveOrderClearKiller(void);
void moveOrder(smoveList* pMoveList, int depth, int filteeQuies, smoveList* pvMoves, negaMaxConf* pStat);
void moveOrderAddKiller(smove* pMove, int depth);

#endif /* SRC_MOVEORDER_H_ */
