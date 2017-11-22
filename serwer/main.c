/*****************************************************************************
*	APPLICATION:	Serwer
*	AUTHOR(S):		Damian Rakowski, Drobne poprawki Denis Lewko
*****************************************************************************/
#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
DWORD WINAPI MessageHandler(void* sd_);
BOOLEAN MessageIncomingPackets(void* params);
typedef struct ClientInfo {
	int clientId;
	SOCKET clientSocket;
	char* ip;
	BOOLEAN hat;
	CRITICAL_SECTION cr;
}CLIENT_INFO;


/******************************************************************************
*	THE MAIN ENTRY POINT TO THE PROGRAM.
*
*	input:	void
*	return:	The code of the reason the process was terminated for.
*			The value defaults to zero.
******************************************************************************/
int __cdecl main(void)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKADDR_IN addr;
	CLIENT_INFO clientInfo;
	clientInfo.hat = TRUE;
	int addrlen = sizeof(addr);
	int clientId = 0;

	struct addrinfo *result = NULL;
	struct addrinfo hints;
	int client_id = 0;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

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
	InitializeCriticalSection(&clientInfo.cr);
	while (TRUE) {
		ClientSocket = accept(ListenSocket, (SOCKADDR*)&addr, &addrlen);
		char* ip = inet_ntoa(addr.sin_addr);
		/*printf("[Klient - %s] \n ",ip);*/
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		else {
			DWORD nThreadID;
			clientInfo.clientId = clientId++;
			clientInfo.clientSocket = ClientSocket;
			clientInfo.ip = ip;
			CreateThread(0, 0, MessageHandler, &clientInfo, 0, &nThreadID);


		}
	}
	DeleteCriticalSection(&clientInfo.cr);
	closesocket(ListenSocket);


	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}


	//// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}


BOOLEAN MessageIncomingPackets(void *params)
{

	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
	int i = 0;
	int defaultListSize = 0;

	char *itemList = malloc(defaultListSize);
	char *tmp = NULL;

	CLIENT_INFO *info = params;

	SOCKET sd = info->clientSocket;
	int clientId = info->clientId;
	char* clientIp = info->ip;

	do {
		iResult = recv(sd, recvbuf, recvbuflen, 0);

		if (iResult > 0) {

			defaultListSize += iResult;
			tmp = realloc(itemList, defaultListSize);

			if (tmp != NULL) {
				itemList = tmp;
			}
			else {
				free(itemList);
				printf("Error allocating memory!\n");
				return 1;
			}

			EnterCriticalSection(&info->cr);
			if ((recvbuf[0] == 'b' && recvbuf[1] == 'a' && recvbuf[2] == 'n' && recvbuf[3] == 'a' && recvbuf[4] == 'n' && recvbuf[5] == 'a')) {
				if (info->hat) {
					recvbuf[0] = 'o'; recvbuf[1] = 'r'; recvbuf[2] = 'a'; recvbuf[3] = 'n'; recvbuf[4] = 'g'; recvbuf[5] = 'e';
					info->hat = FALSE;
				}
				else {
					info->hat = TRUE;
				}


			}
			else if ((recvbuf[0] == 'o' && recvbuf[1] == 'r' && recvbuf[2] == 'a' && recvbuf[3] == 'n' && recvbuf[4] == 'g' && recvbuf[5] == 'e')) {
				if (info->hat) {
					recvbuf[0] = 'o'; recvbuf[1] = 'r'; recvbuf[2] = 'a'; recvbuf[3] = 'n'; recvbuf[4] = 'g'; recvbuf[5] = 'e';
				}

			}
			LeaveCriticalSection(&info->cr);


			itemList[i] = recvbuf[0];
			itemList[i + 1] = recvbuf[1];
			itemList[i + 2] = recvbuf[2];
			itemList[i + 3] = recvbuf[3];
			itemList[i + 4] = recvbuf[4];
			itemList[i + 5] = recvbuf[5];
			itemList[i + 6] = '\n';

			i += iResult;

			printf("[Klient # %d - ip # %s] \n", clientId, clientIp);

			if (info->hat) {
				printf("Stan kapelusza: %s \n", "Wlozony");
			}
			else {
				printf("Stan kapelusza: %s \n", "Zdjety");
			}

			printf("Aktualna lista zakupow: \n");
			for (int l = 0; l < i; l++) {
				printf("%c", itemList[l]);
			}

			printf("\n");



			// Echo the buffer back to the sender
			int iSendResult;
			if (iResult > 0) {
				iSendResult = send(sd, recvbuf, iResult, 0);
			}
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(sd);
				WSACleanup();
				return 1;
			}

		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			closesocket(sd);

		}

	} while (iResult > 0);
	return TRUE;
}


DWORD WINAPI MessageHandler(void* sd_)
{
	CLIENT_INFO* info = sd_;
	int clientId = info->clientId;
	char* clientIp = info->ip;
	int nRetval = 0;
	SOCKET sd = info->clientSocket;

	if (!MessageIncomingPackets(info)) {
		printf("Messge incoming packets failed: %d\n", WSAGetLastError());
		closesocket(sd);
		nRetval = 1;
	}

	return nRetval;
}
