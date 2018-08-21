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

sboard uciBoard;

void uciLog(char* s) {
	FILE * pFile;
	pFile = fopen("logUci.txt", "a");
	fprintf(pFile, "%s\n", s);
	fclose(pFile);
}

void uciParseGo(char* str) {

	// uciGo();
	smoveList mliste;


	boardGenerateAllLegalMoves(&uciBoard, &mliste);


	printf("bestmove ");
	movePrintShort(&mliste._sMoveList[0]);
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
			prom=0;
		}
		from = fromX + 8 * fromY;
		to = toX + 8 * toY;

		foundMove = 0;
		moveListInit(&mliste);
		boardGenerateAllMoves(&uciBoard, &mliste);
		for (int ii = 0; ii < mliste._nbrMove; ii++) {
			if ((from == MOVE_FROM(mliste._sMoveList[ii]._move)) && (to == MOVE_TO(mliste._sMoveList[ii]._move)) && (prom == MOVE_PIECE_PROMOTION(mliste._sMoveList[ii]._move)) ) {
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

