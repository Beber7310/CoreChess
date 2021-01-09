/*
 * uci.c
 *
 *  Created on: 3 août 2018
 *      Author: Bertrand
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "board.h"
#include "move.h"
#include "search.h"
#include "perft.h"
#include "uci.h"
#include "tcpserver.h"

sboard uciBoard;


int uciOptionHashSize = 128 * 1024 * 1024;
int uciOptionQuiesence = 0;

void uciLog(char* s) {
	FILE* pFile;
	pFile = fopen("logUci.txt", "a");
	fprintf(pFile, "%s\n", s);
	fclose(pFile);
}

void uciParseGo(char* str) {
	int wtime = 0;
	int btime = 0;
	int mtime = 0;
	int movestogo = 0;
	char res[64];
	char strMove[64];
	//
	//	go wtime 300000 btime 300000 movestogo 35
	//
	char* token;
	token = strtok(str, " ");

	while (token != NULL) {
		if (strncmp("wtime", token, sizeof("wtime") - 1) == 0) {
			token = strtok(NULL, " ");
			wtime = atoi(token);
		}
		if (strncmp("btime", token, sizeof("btime") - 1) == 0) {
			token = strtok(NULL, " ");
			btime = atoi(token);
		}
		if (strncmp("movestogo", token, sizeof("movestogo") - 1) == 0) {
			token = strtok(NULL, " ");
			movestogo = atoi(token);
		}
		if (strncmp("movetime", token, sizeof("movetime") - 1) == 0) {
			token = strtok(NULL, " ");
			mtime = atoi(token);
		}

		token = strtok(NULL, " ");
	}

	searchStat stat;
	smove mv = searchStart(&uciBoard, wtime, btime, mtime, movestogo, &stat);
	movePrintShort(&mv, &strMove);
	sprintf(res, "bestmove %s \n", strMove);
	printTcp(res);
	
}

void uciParseOption(char* str) {
	//setoption name OwnBook value false

	char optionName[64] = "";
	char optionValue[64] = "";
	char* token;


	token = strtok(str, " ");
	if (strncmp("name", token, sizeof("name") - 1) == 0) {
		token = strtok(NULL, " ");
		strncpy(optionName, token, sizeof(optionName) - 1);		
	}

	token = strtok(NULL, " ");
	if (strncmp("value", token, sizeof("value") - 1) == 0) {
		token = strtok(NULL, " ");
		strncpy(optionValue, token, sizeof(optionValue) - 1);
	}



	if (strncmp("Hash", optionName, sizeof("Hash") - 1) == 0) {
		uciOptionHashSize = atoi(optionValue);
		ttInit(uciOptionHashSize);
	}
	if (strncmp("Quiescence", optionName, sizeof("Quiescence") - 1) == 0) {
		if (strncmp(optionValue, "fasle", sizeof("fasle") - 1) == 0)
		{
			uciOptionQuiesence = 0;
		}
		else
		{
			uciOptionQuiesence = 1;
		}

	}

}
void uciParseMove(char* str) {

	char* token;
	int from, to, prom;
	int foundMove;
	smoveList mliste;
	token = strtok(str, " ");

	while (token != NULL) {
		int fromX, fromY, toX, toY;

		fromX = token[0] - 'a';
		fromY = token[1] - '1';

		toX = token[2] - 'a';
		toY = token[3] - '1';

		switch (token[4]) {
		case 'q':
			prom = QUEEN;
			break;
		case 'r':
			prom = ROOK;
			break;
		case 'b':
			prom = BISHOP;
			break;
		case 'n':
			prom = KNIGHT;
			break;
		default:
			prom = 0;
		}
		from = fromX + 8 * fromY;
		to = toX + 8 * toY;

		foundMove = 0;
		moveListInit(&mliste);
		boardGenerateAllMoves(&uciBoard, &mliste);
		for (int ii = 0; ii < mliste._nbrMove; ii++) {
			if ((from == MOVE_FROM(mliste._sMoveList[ii]._move)) && (to == MOVE_TO(mliste._sMoveList[ii]._move))
				&& (prom == MOVE_PIECE_PROMOTION(mliste._sMoveList[ii]._move))) {
				doMove(&uciBoard, &mliste._sMoveList[ii]);
				foundMove++;
			}
		}
		if (!foundMove) {
			uciLog("No move found!!!");
		}
		token = strtok(NULL, " ");
	}
}

void uciParsePosition(char* str) {

	char* token;
	token = strtok(str, " ");

	while (token != NULL) {
		if (strncmp("startpos", token, sizeof("startpos") - 1) == 0) {
			boardInit(&uciBoard);
		}
		if (strncmp("moves", token, sizeof("moves") - 1) == 0) {
			token = strtok(NULL, "");
			uciParseMove(token);
		}
		token = strtok(NULL, " ");
	}
}

void uciParseCmd(char* str) {
	char* token;
	FILE* pFile;
	pFile = fopen("logUci.txt", "a");
	fprintf(pFile, "GUI CMD> %s", str);
	fclose(pFile);

	token = strtok(str, " ");

	if (strncmp("quit", token, sizeof("quit") - 1) == 0) {
		exit(0);
	}
	else if (strncmp("ucinewgame", token, sizeof("ucinewgame") - 1) == 0) {
		// rien :)
	}
	else if (strncmp("uci", token, sizeof("uci") - 1) == 0) {
		printTcp("option name Ponder type check default true\n");
		if (uciOptionQuiesence)
			printTcp("option name Quiescence type check default true\n");
		else
			printTcp("option name Quiescence type check default false\n");

		printTcp("option name OwnBook type check default true\n");
		printTcp("option name Hash type spin default 48 min 0 max 2048\n");
		printTcp("option name PawnHash type spin default 256 min 0 max 4096\n");
		printTcp("option name EvalHash type spin default 16 min 0 max 1024\n");
		printTcp("option name NumBeers type spin default 0 min 0 max 10\n");
		printTcp("option name NumThreads type spin default 1 min 1 max 8\n");
		printTcp("option name LogStats type check default false");
		printTcp("option name TraceLevel type spin default 0 min 0 max 3\n");
		printTcp("option name TimeBuffer type spin default 0 min 0 max 60\n");

		printTcp("id name CoreChess 1.0\n");
		printTcp("uciok\n");
	}
	else if (strncmp("setoption", token, sizeof("setoption") - 1) == 0) {
		token = strtok(NULL, "");
		uciParseOption(token);
	}
	else if (strncmp("isready", token, sizeof("isready") - 1) == 0) {
		printTcp("readyok\n");
	}
	else if (strncmp("position", token, sizeof("position") - 1) == 0) {
		token = strtok(NULL, "");
		uciParsePosition(token);
	}
	else if (strncmp("go", token, sizeof("go") - 1) == 0) {
		token = strtok(NULL, "");
		uciParseGo(token);
	}
	else if (strncmp("perft", token, sizeof("perft") - 1) == 0) {
		perftCheckFile("perftcheck.epd", 4);
	}
	else if (strncmp("puz2", token, sizeof("puz2") - 1) == 0) {
		puzzlzCheckFile("mat2.epd", 2);
	}
	else if (strncmp("puz3", token, sizeof("puz3") - 1) == 0) {
		puzzlzCheckFile("mat3.epd", 4);
	}
	else if (strncmp("puz4", token, sizeof("puz4") - 1) == 0) {
		puzzlzCheckFile("mat4.epd", 4);
	}
	else if (strncmp("exit", token, sizeof("exit") - 1) == 0) {
		exit(0);
	}

}

void uciBanner() {
	char str[256];

	printTcp("------------------------------------------------------------\n");
	printTcp("-                                                          -\n");
	printTcp("-                    CoreChess                             -\n");
	printTcp("-                                                          -\n");
	printTcp("------------------------------------------------------------\n");
	sprintf(str, "\n");
	printTcp(str);
	sprintf(str, "Compiled on %s at %s\n", __DATE__, __TIME__);
	printTcp(str);
	sprintf(str, "\n");
	printTcp(str);
}

void main_UCI() {
	char str[1024];
	FILE* pFile;

	pFile = fopen("logUci.txt", "a");

	fclose(pFile);

	setvbuf(stdout, NULL, _IONBF, 0);
	uciBanner();
	while (1) {
		fgets(str, 1024, stdin);

		uciParseCmd(str);
	}
}
