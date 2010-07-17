/**
 * Copyright (C) 2010 Rafael Fernández López <ereslibre@ereslibre.es>
 *
 * This program is licensed under the GPLv3 or higher.
 *
 * Extremely simple FTP client (it does nothing, academic purposes).
 */

#include <time.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

static void sendCommand(int sock, const char *command)
{
	printf("*** Sending command: %s", command);
	send(sock, command, strlen(command), 0);
}

static void recvCommand(int sock)
{
	char *buf = (char*) calloc(1025, sizeof(char));
	printf("*** Received:\n");

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(sock, &readfds);

	struct timespec time;
	time.tv_sec = 0;
	time.tv_nsec = 500000000;

	while (pselect(sock + 1, &readfds, 0, 0, &time, 0) > 0) {
		recv(sock, buf, 1024, 0);
		printf("%s", buf);
		memset(buf, '\0', 1025);
	}

	free(buf);
}

int main()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	struct hostent *const host = gethostbyname("ftp.kde.org");

	if (!host) {
		printf("!!! Call to gethostbyname failed: %d\n", h_errno);
		return 1;
	} else {
		printf("*** Host is: %s\n", inet_ntoa(*((struct in_addr*) host->h_addr)));
	}

	struct sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(21);
	destAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*) host->h_addr)));
	memset(&(destAddr.sin_zero), '\0', 8);

	if (!connect(sock, (struct sockaddr*) &destAddr, sizeof(struct sockaddr))) {
		sendCommand(sock, "USER anonymous\r\n");
		recvCommand(sock);
		sendCommand(sock, "PASS\r\n");
		recvCommand(sock);
		sendCommand(sock, "CWD /\r\n");
		recvCommand(sock);
		sendCommand(sock, "PWD\r\n");
		recvCommand(sock);
		sendCommand(sock, "CWD /pub/kde\r\n");
		recvCommand(sock);
		sendCommand(sock, "PWD\r\n");
		recvCommand(sock);
		sendCommand(sock, "PASV\r\n");
		recvCommand(sock);
		sendCommand(sock, "LIST\r\n");
		recvCommand(sock);
	} else {
		printf("!!! Could not connect to ftp.kernel.org\n");
		return 1;
	}

	close(sock);

	return 0;
}

