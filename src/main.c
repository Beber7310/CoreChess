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

	//perftRun("r4k2/8/8/8/8/8/8/4K3 b q - 0 1 ", 3,92);
	perftRun("5k2/8/8/8/8/8/8/r3K3 b q - 0 1 ", 1,92);


	//perftCheckFile("perftcheck.epd",3);
}

