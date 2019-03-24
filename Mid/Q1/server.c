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
	int port, limit;
} sfd_obj[numSFD];

int currConnected[numSFD] = {0};

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
	char port[buffSize], lim[buffSize];
	int ind = 0, i;
	for(i = 0; i < strlen(buff); i++)
		if(buff[i] == ' ')
			break;
		else
			port[ind++] = buff[i];
	sfd_obj[currSize].port = atoi(port);

	i++, ind = 0;
	for(; i < strlen(buff); i++)
		if(buff[i] == ' ')
			break;
		else
			lim[ind++] = buff[i];
	sfd_obj[currSize].limit = atoi(lim);

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

void initializeISS(int toSkip)
{
	int opt = 1;
	char buff[buffSize];
	FILE* fd = fopen("config.txt", "r");
	while(fgets(buff, sizeof(buff), fd))
	{
		if(toSkip > 0)
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

int findInd(char str[10])
{
	for(int i = 0; i < currSize; i++)
		if(strcmp(sfd_obj[i].target, str) == 0)
			return i;
}

void procEnd()
{
	currConnected[findInd("./s1")]--;
}

void* S2(void* arg)
{
	int ind = findInd("./s2");
	currConnected[ind]++;

	int* nsfd = (int*) arg;
	char str[100], ans[10];

	recv(*nsfd, str, sizeof(str), 0);
	sprintf(ans, "%ld", strlen(str));

	sleep(4);

	send(*nsfd, ans, strlen(ans), 0);
	close(*nsfd);

	currConnected[ind]--;
}

void main()
{
	printf("%i\n", getpid());
	signal(SIGUSR1, procEnd);
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

		int s = select(returnMax(), &rfds, NULL, NULL, NULL);
		for(int i = 0; i < currSize; i++)
		{
			if(FD_ISSET(sfd_obj[i].sfd, &rfds))
			{	
				if(strcmp(sfd_obj[i].prot, "tcp") == 0 && (currConnected[i] < sfd_obj[i].limit || sfd_obj[i].limit == -1))
				{
					int nsfd = accept(sfd_obj[i].sfd, (struct sockaddr*)&address[i], (socklen_t*)&addrlen);
					if(strcmp(sfd_obj[i].type, "extern") == 0)
					{
						currConnected[i]++;
						if(fork() == 0)
						{
							closeAllSFD();

							char old_stdout[10], old_stdin[10];
							sprintf(old_stdout, "%d", dup(1));
							sprintf(old_stdin, "%d", dup(0));

							dup2(nsfd, 0);
							dup2(nsfd, 1);
							char *args[] = {sfd_obj[i].target, old_stdin, old_stdout, NULL};
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
						pthread_create(&w, NULL, S2, &tmp);
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