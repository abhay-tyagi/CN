#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/select.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define numSFD 100
#define buffSize 100

struct sfd_info {
	int sfd;
	char type[10], prot[10], target[10];
	int port;
} sfd_obj[numSFD];

struct sockaddr_in address[numSFD];
int currSize = 0;

int returnMax()
{
	int maxNum = -1;
	for(int i = 0; i < currSize; i++)
		if(sfd_obj[i].sfd > maxNum)
			maxNum = sfd_obj[i].sfd;

	return maxNum + 1;
}

void getSfdInfo(char buff[])
{
	char port[buffSize];
	int ind = 0, i;
	for(i = 0; i < strlen(buff); i++)
		if(buff[i] == ' ')
			break;
		else
			port[ind++] = buff[i];
	sfd_obj[currSize].port = atoi(port);

	for(int j = 0; j < 3; j++)
	{
		i++;
		ind = 0;
		for(; i < strlen(buff); i++)
		{
			if(buff[i] == ' ')
				break;
			else
			{
				if(j == 0)
					sfd_obj[currSize].prot[ind++] = buff[i];
				else if(j == 1)
					sfd_obj[currSize].type[ind++] = buff[i];
				else if(j == 2)
					sfd_obj[currSize].target[ind++] = buff[i];	 
			}
		}
	}
}

void closeAllSFD()
{
	for(int i = 0; i < currSize; i++)
		close(sfd_obj[i].sfd);
}

void initializeISS(int toSkip = currSize)
{
	int opt = 1;
	char buff[buffSize];
	FILE* fd = fopen("config.txt", "r");
	while(fgets(buff, sizeof(buff), fd))
	{
		if(toSkip--)
			continue;

		getSfdInfo(buff);
		address[currSize].sin_family = AF_INET;
		address[currSize].sin_addr.s_addr = INADDR_ANY;
		address[currSize].sin_port = htons(sfd_obj[currSize].port);

		if(strcmp(sfd_obj[currSize].prot, "tcp") == 0)
		{
			sfd_obj[currSize].sfd = socket(AF_INET, SOCK_STREAM, 0);
			setsockopt(sfd_obj[currSize].sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
			bind(sfd_obj[currSize].sfd, (struct sockaddr*)&address[currSize], sizeof(address[currSize]));
			listen(sfd_obj[currSize].sfd, 10);
		}
		else if(strcmp(sfd_obj[currSize].prot, "udp") == 0)
		{
			sfd_obj[currSize].sfd = socket(AF_INET, SOCK_DGRAM, 0);
			bind(sfd_obj[currSize].sfd, (struct sockaddr*)&address[currSize], sizeof(address[currSize]));
		}
		currSize++;
	}
	fclose(fd);
}

void* serveReq(void* arg)
{
	char msg[] = "Thread responded";
	int* nsfd = (int*) arg;
	for(int i = 0; i < 3; i++)
	{
		send(*nsfd, msg, strlen(msg), 0);
		sleep(4);
	}
	close(*nsfd);
}

void main()
{
	signal(SIGUSR1, addNewSFD);
	initializeISS(0);
	int addrlen = sizeof(address[0]);

	struct timeval tv;
	fd_set rfds;
	FD_ZERO(&rfds);

	while(1)
	{
		for(int i = 0; i < currSize; i++)
			FD_SET(sfd_obj[i].sfd, &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		int s = select(returnMax(), &rfds, NULL, NULL, &tv);
		for(int i = 0; i < currSize; i++)
		{
			if(FD_ISSET(sfd_obj[i].sfd, &rfds))
			{
				if(strcmp(sfd_obj[i].prot, "tcp") == 0)
				{
					int nsfd = accept(sfd_obj[i].sfd, (struct sockaddr*)&address[i], (socklen_t*)&addrlen);
					if(strcmp(sfd_obj[i].type, "extern") == 0)
					{
						if(fork() == 0)
						{
							closeAllSFD();
							dup2(nsfd, 0);
							dup2(nsfd, 1);
							char *args[] = {sfd_obj[i].target, NULL};
							execvp(args[0], args);
							break;
						}				
						else
							close(nsfd);
					}
					else if(strcmp(sfd_obj[i].type, "intern") == 0)
					{
						int tmp = nsfd;
						pthread_t w;
						pthread_create(&w, NULL, serveReq, &tmp);
					}
				}
				else if(strcmp(sfd_obj[i].prot, "udp") == 0)
				{
					int len;
					struct sockaddr_in foreign_addr;
				    memset(&foreign_addr, 0, sizeof(foreign_addr)); 

				    char buf[buffSize];
					recvfrom(sfd_obj[i].sfd, (char *)buf, sizeof(buf), MSG_WAITALL, (struct sockaddr*)&foreign_addr, &len); 

					char response[] = "UDP Response";
					sendto(sfd_obj[i].sfd, response, strlen(response), MSG_CONFIRM, (struct sockaddr*)&foreign_addr, sizeof(foreign_addr)); 
				}
			}
		}
	}
}