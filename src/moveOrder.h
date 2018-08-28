/*
 * moveOrder.h
 *
 *  Created on: Aug 27, 2018
 *      Author: dosdab
 */

#ifndef SRC_MOVEORDER_H_
#define SRC_MOVEORDER_H_

void moveOrderInit(void);
void moveOrder(smoveList* pMoveList,searchStat* pStat );
void moveOrderAddKiller(smove* pMove);

#endif /* SRC_MOVEORDER_H_ */
