/*****************************************************************************
*	APPLICATION:	Klient
*	AUTHOR(S):		Denis Lewko
*****************************************************************************/

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")

int main(int argc, char **argv) {

	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char recvbuf[DEFAULT_BUFLEN];
	char SENDBFR[DEFAULT_BUFLEN];
	int SENDBFR_MAX = 8;
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;


	if (argc != 2) {
		printf("Musisz podac adres ip w parametrze! \n");
		return 1;
	}


	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}


	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {


		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}


		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}

		break;
	
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}


	while (1) {


		do {

			printf("client -> ");
			fgets(SENDBFR, SENDBFR_MAX, stdin);


		}

		while ((!((int)strlen(SENDBFR) == 7 && ((SENDBFR[0] == 'b' && SENDBFR[1] == 'a' && SENDBFR[2] == 'n' && SENDBFR[3] == 'a' && SENDBFR[4] == 'n' && SENDBFR[5] == 'a') || (SENDBFR[0] == 'o' && SENDBFR[1] == 'r' && SENDBFR[2] == 'a' && SENDBFR[3] == 'n' && SENDBFR[4] == 'g' && SENDBFR[5] == 'e')))));


		iResult = send(ConnectSocket, SENDBFR, (int)strlen(SENDBFR), 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			break;
		}


		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Server -> %c%c%c%c%c%c\n\n", recvbuf[0], recvbuf[1], recvbuf[2], recvbuf[3], recvbuf[4], recvbuf[5]);
		else if (iResult == 0)
			printf("Connection closed\n\n");
		else
			printf("recv failed with error: %d\n\n", WSAGetLastError());



	}

	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}