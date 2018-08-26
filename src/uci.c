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

sboard uciBoard;

void uciLog(char* s) {
	FILE * pFile;
	pFile = fopen("logUci.txt", "a");
	fprintf(pFile, "%s\n", s);
	fclose(pFile);
}

void uciParseGo(char* str) {
	int wtime = 0;
	int btime = 0;
	int movestogo = 0;

//
//	go wtime 300000 btime 300000 movestogo 35
//
	char *token;
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
		token = strtok(NULL, " ");
	}

	smove mv = searchStart(&uciBoard, wtime, btime, movestogo);
	printf("bestmove ");
	movePrintShort(&mv);
	printf("\n");

}

void uciParseMove(char* str) {

	char *token;
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
			if ((from == MOVE_FROM(mliste._sMoveList[ii]._move)) && (to == MOVE_TO(mliste._sMoveList[ii]._move)) && (prom == MOVE_PIECE_PROMOTION(mliste._sMoveList[ii]._move))) {
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

	char *token;
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
	char *token;
	FILE * pFile;
	pFile = fopen("logUci.txt", "a");
	fprintf(pFile, "GUI CMD> %s", str);
	fclose(pFile);

	token = strtok(str, " ");

	if (strncmp("quit", token, sizeof("quit") - 1) == 0) {
		exit(0);
	} else if (strncmp("ucinewgame", token, sizeof("ucinewgame") - 1) == 0) {
		// rien :)
	} else if (strncmp("uci", token, sizeof("uci") - 1) == 0) {
		printf("id name CoreChess\n");
		printf("id author Bertrand\n");

		printf("option name option1 type string default\n");
		printf("option name option2 type spin default 1 min 1 max 32\n");

		printf("uciok\n");
	} else if (strncmp("isready", token, sizeof("isready") - 1) == 0) {
		printf("readyok\n");
	} else if (strncmp("position", token, sizeof("position") - 1) == 0) {
		token = strtok(NULL, "");
		uciParsePosition(token);
	} else if (strncmp("go", token, sizeof("go") - 1) == 0) {
		token = strtok(NULL, "");
		uciParseGo(token);
	}else if (strncmp("perft", token, sizeof("perft") - 1) == 0) {
		perftCheckFile("perftcheck.epd", 4);
	}else if (strncmp("puz2", token, sizeof("puz2") - 1) == 0) {
		puzzlzCheckFile("mat2.epd", 2);
	}



}

void main_UCI() {
	char str[1024];
	FILE * pFile;

	pFile = fopen("logUci.txt", "a");

	fclose(pFile);

	setvbuf(stdout, NULL, _IONBF, 0);

	while (1) {
		fgets(str, 1024, stdin);

		uciParseCmd(str);
	}
}

