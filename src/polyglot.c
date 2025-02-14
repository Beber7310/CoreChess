/*
 * This utility looks up the moves and their scores in a Polyglot book
 *
 * Usage:
 * pg_show <book> <hex key>"
 *
 * You can find the hex key of a FEN using pg_key.
 *
 * This code is released in the public domain by Michel Van den Bergh.
 *
 */


#include <stdio.h>
#include <string.h>

#include "board.h"
#include "search.h"
#include "move.h"
#include "movegen.h"
#include "polyglot.h"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;



typedef struct {
	uint64 key;
	uint16 move;
	uint16 weight;
	uint32 learn;
} entry_t;

entry_t entry_none = {
	0, 0, 0, 0
};

char* promote_pieces = " nbrq";

#define MAX_MOVES 100

int int_from_file(FILE* f, int l, uint64* r) {
	int i, c;
	for (i = 0; i < l; i++) {
		c = fgetc(f);
		if (c == EOF) {
			return 1;
		}
		(*r) = ((*r) << 8) + c;
	}
	return 0;
}

int entry_from_file(FILE* f, entry_t* entry) {
	int ret;
	uint64 r;
	ret = int_from_file(f, 8, &r);
	if (ret) return 1;
	entry->key = r;
	ret = int_from_file(f, 2, &r);
	if (ret) return 1;
	entry->move = r;
	ret = int_from_file(f, 2, &r);
	if (ret) return 1;
	entry->weight = r;
	ret = int_from_file(f, 4, &r);
	if (ret) return 1;
	entry->learn = r;
	return 0;
}

int find_key(FILE* f, uint64 key, entry_t* entry) {
	int first, last, middle;
	entry_t first_entry = entry_none, last_entry, middle_entry;
	first = -1;
	if (fseek(f, -16, SEEK_END)) {
		*entry = entry_none;
		entry->key = key + 1; //hack
		return -1;
	}
	last = ftell(f) / 16;
	entry_from_file(f, &last_entry);
	while (1) {
		if (last - first == 1) {
			*entry = last_entry;
			return last;
		}
		middle = (first + last) / 2;
		fseek(f, 16 * middle, SEEK_SET);
		entry_from_file(f, &middle_entry);
		if (key <= middle_entry.key) {
			last = middle;
			last_entry = middle_entry;
		}
		else {
			first = middle;
			first_entry = middle_entry;
		}
	}
}

void move_to_string(char move_s[6], uint16 move) {
	int f, fr, ff, t, tr, tf, p;
	f = (move >> 6) & 077;
	fr = (f >> 3) & 0x7;
	ff = f & 0x7;
	t = move & 077;
	tr = (t >> 3) & 0x7;
	tf = t & 0x7;
	p = (move >> 12) & 0x7;
	move_s[0] = ff + 'a';
	move_s[1] = fr + '1';
	move_s[2] = tf + 'a';
	move_s[3] = tr + '1';
	if (p) {
		move_s[4] = promote_pieces[p];
		move_s[5] = '\0';
	}
	else {
		move_s[4] = '\0';
	}
	if (!strcmp(move_s, "e1h1")) {
		strcpy(move_s, "e1g1");
	}
	else  if (!strcmp(move_s, "e1a1")) {
		strcpy(move_s, "e1c1");
	}
	else  if (!strcmp(move_s, "e8h8")) {
		strcpy(move_s, "e8g8");
	}
	else  if (!strcmp(move_s, "e8a8")) {
		strcpy(move_s, "e8c8");
	}
}


void move_to_smove(smove* theMove, uint16 move) {
	int f, fr, ff, t, tr, tf, p;
	f = (move >> 6) & 077;
	fr = (f >> 3) & 0x7;
	ff = f & 0x7;
	t = move & 077;
	tr = (t >> 3) & 0x7;
	tf = t & 0x7;
	p = (move >> 12) & 0x7;

	theMove->_move = 0;

	theMove->_move |= (ff + fr * 8) << 9;
	theMove->_move |= (tf + tr * 8) << 15;


	if (p) {
		switch (p)
		{
		case 1:
			theMove->_move | KNIGHT << 3;
			break;
		case 2:
			theMove->_move | BISHOP << 3;
			break;
		case 3:
			theMove->_move | ROOK << 3;
			break;
		case 4:
			theMove->_move | QUEEN << 3;
			break;
		}
	}

	// white short      e1h1
	// white long       e1a1
	// black short      e8h8
	// black long       e8a8

	// if (!strcmp(move_s, "e1h1")) {
	if ((MOVE_FROM(theMove->_move) == e1) && (MOVE_TO(theMove->_move) == h1)) {
		moveBuildCastle(&theMove->_move, e1, g1, KSIDE_CASTLE);
	}
	//else  if (!strcmp(move_s, "e1a1")) {
	else if ((MOVE_FROM(theMove->_move) == e1) && (MOVE_TO(theMove->_move) == a1)) {
		moveBuildCastle(&theMove->_move, e1, c1, QSIDE_CASTLE);
	}
	//else  if (!strcmp(move_s, "e8h8")) {
	else if ((MOVE_FROM(theMove->_move) == e8) && (MOVE_TO(theMove->_move) == h8)) {
		moveBuildCastle(&theMove->_move, e8, g8, KSIDE_CASTLE);
	}
	//else  if (!strcmp(move_s, "e8a8")) {
	else if ((MOVE_FROM(theMove->_move) == e8) && (MOVE_TO(theMove->_move) == a8)) {
		moveBuildCastle(&theMove->_move, e8, c8, QSIDE_CASTLE);
	}
}

int polyglot_listMove(uint64 key, smove* pMove) {
	//uint64 key= 0x463b96181691fc9c;
	FILE* f;
	entry_t entry;
	int offset;
	char* file_name = "book.bin";
	// uint64 key;
	entry_t entries[MAX_MOVES];
	int count = 0;
	int ret, i;
	char move_s[6];
	int total_weight;
	char str_tmp[256];

	f = fopen(file_name, "rb");
	if (!f) {
		perror(file_name);
		printf("Unable to find opening book %s\n", file_name);
		return NULL;
	}
	offset = find_key(f, key, &entry);
	if (entry.key != key) {
#ifdef _MSC_VER
		printf("%016I64x: No such key\n", key);
#else
		printf("%016llx: No such key\n", key);
#endif  
		fclose(f);
		return 0;
	}
	entries[0] = entry;
	count = 1;
	fseek(f, 16 * (offset + 1), SEEK_SET);
	while (1) {
		ret = entry_from_file(f, &entry);
		if (ret) {
			break;
		}
		if (entry.key != key) {
			break;
		}
		if (count == MAX_MOVES) {
			printf("Too many moves in this position (max=%d)\n", MAX_MOVES);
			fclose(f);
			return 0;
		}
		entries[count++] = entry;
	}
	total_weight = 0;
	for (i = 0; i < count; i++) {
		total_weight += entries[i].weight;
	}
	for (i = 0; i < count; i++) {
		move_to_string(move_s, entries[i].move);
		sprintf(str_tmp,"move=%s weight=%5.2f%%\n", move_s, 100 * ((double)entries[i].weight / (double)total_weight));
		printTcp(str_tmp);
	}

	
	move_to_smove(pMove, entries[0].move);


	fclose(f);
	return 1;
}

