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

#define PORT 9999

struct service {
	char name[10];
	int port, sfd, pid;
} s[100];

int n = 0;

struct sockaddr_in clients[100];
int cl = 0;

int giveMax()
{
	int max = 0;
	for(int i = 0; i < n; i++)
		if(s[i].sfd > max)
			max = s[i].sfd;

	return max;
}

void informAllClients()
{
	int opt = 1;  	
	int sfd = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	struct sockaddr_in address;
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(PORT);

	bind(sfd, (struct sockaddr*)&address, sizeof(address));

	for(int i = 0; i < cl; i++)
		sendto(sfd, s[n-1].name, strlen(s[n-1].name), MSG_DONTWAIT, (struct sockaddr*)&clients[i], sizeof(clients[i])); 
}

void createNewService(char buffer[100])
{
	char port[10], name[10];
	int i, ind = 0, opt = 1;
	for(i = 0; i < strlen(buffer); i++)
	{
		if(buffer[i] == ',')
		{
			s[n].port = atoi(port);
			i++;
			break;
		}
		else
			port[ind++] = buffer[i];
	}

	ind = 0;
	for(; i < strlen(buffer); i++)
		if(buffer[i] != '\n')
			name[ind++] = buffer[i];

	strcpy(s[n].name, name);

	struct sockaddr_in address;
	int addrlen = sizeof(address);
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(s[n].port); 
	s[n].sfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(s[n].sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	bind(s[n].sfd, (struct sockaddr*)&address, sizeof(address));
	listen(s[n].sfd, 10);

	n++;
	informAllClients();
}

void* takeEnq()
{
	int opt = 1;  	
	int sfd = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	struct sockaddr_in address;
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(PORT);

	bind(sfd, (struct sockaddr*)&address, sizeof(address));	

	while(1)
	{
	    struct sockaddr_in foreign_addr;
		int addrlen = sizeof(foreign_addr);

		char buffer[100];
		recvfrom(sfd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr*)&foreign_addr, (socklen_t*)&addrlen); 
		clients[cl++] = foreign_addr;

		char servs[1000] = {0}, i = 0;
		for(int j = 0; j < n; j++)
		{
			for(int k = 0; k < strlen(s[j].name); k++)
				servs[i++] = s[j].name[k];
			servs[i++] = '\t';
		}

		sendto(sfd, servs, strlen(servs), MSG_CONFIRM, (struct sockaddr*)&foreign_addr, sizeof(foreign_addr));
	}
}

void main() 
{
	pthread_t w;
	pthread_create(&w, NULL, takeEnq, NULL);

	fd_set rfds;
	FD_ZERO(&rfds);

	while(1)
	{
		FD_SET(0, &rfds);
		for(int i = 0; i < n; i++)
			FD_SET(s[i].sfd, &rfds);

		int ind = select(giveMax() + 1, &rfds, NULL, NULL, NULL);

		if(FD_ISSET(0, &rfds))
		{
		    char buffer[100] = {0};
			read(0, buffer, sizeof(buffer));
			createNewService(buffer);

			int c = fork();
			if(c == 0)
			{
				char ans[10];
				sprintf(ans, "%i", s[n-1].sfd);
				char* args[] = {s[n-1].name, ans, NULL};
				execvp(args[0], args);
			}
			else
				s[n-1].pid = c;
		}

		for(int i = 0; i < n; i++)
			if(FD_ISSET(s[i].sfd, &rfds))
				kill(s[i].pid, SIGUSR1);
	}

	while(1);
	pthread_join(w, NULL);
} 
