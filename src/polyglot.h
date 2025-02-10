#pragma once

#ifdef _MSC_VER
typedef unsigned __int64 uint64;
#else
typedef unsigned long long int uint64;
#endif

int polyglot_listMove(uint64 key, smove* pMove);
uint64 polyglot_hash(char* fen);