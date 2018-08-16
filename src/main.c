/*
 * main.c
 *
 *  Created on: 7 août 2018
 *      Author: Bertrand
 */

#include "board.h"
#include "movegen.h"
#include "perft.h"

int main() {
	moveGenInit();

	perftRun("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 1,48);


	//perftCheck();

}

