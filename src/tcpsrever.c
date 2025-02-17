#undef UNICODE
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "movegen.h"
#include "perft.h"
#include "uci.h"
#include "search.h"
#include "moveOrder.h"
#include "zobrist.h"
#include "transposition.h"


#ifdef _MSC_VER
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
#endif


#define DEFAULT_BUFLEN 2048
#define DEFAULT_PORT "13000"


#ifdef _MSC_VER
SOCKET ClientSocket = 0;
#else
int ClientSocket = 0;
#endif

#ifdef _WIN32
int main_TCP(void)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	//  SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;



	printf("  Start Server                                 \n");
	printf("                                   \n");
	printf("                                   \n");
	printf("                                   \n");

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	while (1)
	{
		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}


		printf("[TCP] Receive connection\n");


		// Receive until the peer shuts down the connection
		do {

			iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);

			if (iResult > 0) {
				printf("Bytes received: %d\n", iResult);
				recvbuf[iResult] = 0;
				uciParseCmd(recvbuf);

				// Echo the buffer back to the sender
				iSendResult = send(ClientSocket, "tcp_end\n", sizeof("tcp_end\n"), 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					return 1;
				}
				// printf("Bytes sent: %d\n", iSendResult);
			}
			else if (iResult == 0)
				printf("Connection closing...\n");
			else {
				int err = WSAGetLastError();
				printf("recv failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				if (err != WSAECONNRESET)
				{
					WSACleanup();
					return 1;
				}

			}

		} while (iResult > 0);

		// shutdown the connection since we're done
		iResult = shutdown(ClientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			/*WSACleanup();
			return 1;*/
		}

		// cleanup
		closesocket(ClientSocket);

	}

	// No longer need server socket
	closesocket(ListenSocket);
	WSACleanup();
	return 0;
}

#endif

void printTcp(char* str)
{
	FILE* pFile;

	if (ClientSocket)
	{
#ifdef _MSC_VER
		int iSendResult;
		iSendResult = send(ClientSocket, str, (int)strlen(str), 0);
		if (iSendResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
		}
		printf(str);
		pFile = fopen(logFileName, "a");
		fprintf(pFile, "GUI RSP> %s", str);
		fclose(pFile);
#endif
	}
	else
	{
		pFile = fopen(logFileName, "a");
		if (pFile) {
			fprintf(pFile, "GUI RSP> %s", str);
			fclose(pFile);
		}
		else
		{
			printf("Unable to open logUci.txt\n");
		}
		printf(str);
	}
}
