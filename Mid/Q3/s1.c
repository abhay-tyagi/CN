#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <signal.h>
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#define PORT 8080

char sf[20];

void* takeCare()
{
	int opt = 1;  	
	int sfd = atoi(sf);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	struct sockaddr_in address;
	int addrlen = sizeof(address);
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(PORT); 
	
	bind(sfd, (struct sockaddr*)&address, sizeof(address));
	listen(sfd, 10);

	int nsfd = accept(sfd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

	char response[] = "Yeah, S1 got you";
	send(nsfd, response, strlen(response), 0); 

	close(nsfd);
	close(sfd);
}

void handler()
{
	pthread_t w;
	pthread_create(&w, NULL, takeCare, NULL);
}

void main(int argc, char* argv[])
{
	printf("S1 started\n");

	strcpy(sf, argv[1]);
	signal(SIGUSR1, handler);

	while(1);
}