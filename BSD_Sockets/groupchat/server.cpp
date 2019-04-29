#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include <map>

using namespace std;
#define PORT 8080

map< int, vector<int> > grpMems;
int nsfd[1000], l = 0;
struct sockaddr_in address;
int addrlen = sizeof(address), opt = 1, sfd;

void* doAccept(void* args)
{
	while(1)
	{
		nsfd[l] = accept(sfd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		l++;
		printf("Added nsfd %i\n", nsfd[l-1]);
	}
}

int maxAll()
{
	int max = -1;
	for(int i = 0; i < l; i++)
		max = (nsfd[i] >= max) ? nsfd[i] : max;

	return max + 1;
}

int alreadyMem(int fd)
{
	for(map<int,vector<int>>::iterator it = grpMems.begin(); it != grpMems.end(); it++)
	{
		for(int i = 0; i < it->second.size(); i++)
			if(fd == it->second[i])
				return it->first;
	}

	return 0;
}

int main()
{
	address.sin_port = htons(PORT);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	bind(sfd, (struct sockaddr*)&address, sizeof(address));
	listen(sfd, 10);

	pthread_t w;
	pthread_create(&w, NULL, doAccept, NULL);

	struct timeval tv;
	fd_set rfds;
	FD_ZERO(&rfds);

	while(1)
	{
		for(int i = 0; i < l; i++)
			if(nsfd[i] != -1)
				FD_SET(nsfd[i], &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		int res = select(maxAll(), &rfds, NULL, NULL, &tv);
		for(int i = 0; i < l; i++)
		{
			if(nsfd[i] != -1 && FD_ISSET(nsfd[i], &rfds))
			{
				int ind;
				char buff[1000] = {0}, msg[1024];
				int n = recv(nsfd[i], buff, sizeof(buff), 0);
				if(n == 0)
				{
					close(nsfd[i]);
					nsfd[i] = -1;					
				}
				printf("Received %s\n", buff);

				int mem = alreadyMem(nsfd[i]);
				if(buff[1] == 'j' && buff[2] == 'g' && isdigit(buff[3]) && !mem)
				{
					int grp = buff[3] - '0';
					grpMems[grp].push_back(nsfd[i]);
					printf("Added %i to %i\n", nsfd[i], grp);
				}
				else if(!mem)
				{
					printf("Not a member of any group\n");
					continue;
				}

				msg[0] = 'c';
				msg[1] = buff[0];
				msg[2] = ' ';
				msg[3] = '(';
				msg[4] = 'g';
				msg[5] = mem + '0';
				msg[6] = ')';
				msg[7] = ':';
				msg[8] = ' ';
				for(ind = 1; ind < strlen(buff); ind++)
					msg[ind+8] = buff[ind];
				msg[ind+8] = '\0';

				for(int j = 0; j < grpMems[mem].size(); j++)
					if(nsfd[i] != grpMems[mem][j])
						send(grpMems[mem][j], msg, strlen(msg), 0);
			}
		}
	}

	pthread_join(w, NULL);

	return 0;
}