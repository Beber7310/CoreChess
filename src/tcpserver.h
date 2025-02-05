 
#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <stdlib.h>
	#include <stdio.h>
#endif

void printTcp(char* str);

#endif