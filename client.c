/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "29732" // the port client will be connecting to

#define MAXDATASIZE 4096 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	char filename[500];
	FILE *fp;
	int currLocation = 0;
	char character = '?';
	int length = 0;
	unsigned long fileSize = 0;
	unsigned long currSend = 0;
	int mysend = 0;


	printf("argc: %d\n", argc);

	if (argc < 3)
	{
		fprintf(stderr, "Please enter valid inputs (Ex> ./client server-IP-address filename )\n");
		exit(1);
	}
	strcpy(filename, argv[2]);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1)
		{
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			  s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	// *********************************
	char* bufferToServer = (char*)malloc(sizeof(char) * (MAXDATASIZE + 1));
	if(bufferToServer == NULL){
		fprintf(stderr, "bufferToServer is NULL!\n");
		exit(1);
	}
	strcpy(bufferToServer,"");

	// Read from file
	fp = fopen(filename,"r");
	if(fp == NULL){
		fprintf(stderr, "The file is NULL!\n");
	}

	do {
		// Send packets to server
		while( currLocation < MAXDATASIZE && (character = fgetc(fp)) != EOF){
			bufferToServer[currLocation] = character;
			currLocation = currLocation + 1;
		}
		bufferToServer[currLocation] = '\0';
		length  = strlen(bufferToServer);

		while((mysend = send(sockfd, bufferToServer, length, 0)) <= 0);
		// mysend = send(sockfd, bufferToServer, length, 0);
		printf("mysend Val :: %d\n", mysend);

		currSend = currSend + currLocation;
		currLocation = 0;
		strcpy(bufferToServer, "");
	} while(currSend < fileSize);
	fclose(fp);
	free(bufferToServer);
	close(sockfd);
	exit(0);
	
}
