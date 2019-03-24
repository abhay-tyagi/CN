#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#define PORT 8080

int nsfd[1000] = {-1}, l = 0;
struct sockaddr_in address;
int addrlen = sizeof(address), opt = 1, sfd;

void* doAccept()
{
	while(1)
	{
		nsfd[l] = accept(sfd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		l++;
		printf("Added nsfd\n");
	}
}

int maxAll()
{
	int max = -1;
	for(int i = 0; i < l; i++)
		max = (nsfd[i] >= max) ? nsfd[i] : max;

	return max + 1;
}

void main()
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
		{
			if(nsfd[i] != -1)
				FD_SET(nsfd[i], &rfds);
		}
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

				msg[0] = 'c';
				msg[1] = buff[0];
				msg[2] = ':';
				msg[3] = ' ';
				for(ind = 1; ind < strlen(buff); ind++)
					msg[ind+3] = buff[ind];
				msg[ind+3] = '\0';

				for(int j = 0; j < l; j++)
					if(j != i && nsfd[j] != -1)
						send(nsfd[j], msg, strlen(msg), 0);
			}
		}
	}

	pthread_join(w, NULL);
}