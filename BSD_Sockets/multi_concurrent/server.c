#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

const int PORT[3] = {8080, 7070, 7090};

int returnMax(int i1, int i2, int i3)
{
	if(i1 >= i2 && i1 >= i3)
		return i1 + 1;
	if(i2 >= i1 && i2 >= i3)
		return i2 + 1;
	return i3 + 1;
}

void main()
{
	struct sockaddr_in address[3];
	int addrlen = sizeof(address[0]);

	int sfd[3], opt = 1;
	for(int i = 0; i < 3; i++)
	{
		address[i].sin_family = AF_INET;
		address[i].sin_addr.s_addr = INADDR_ANY;
		address[i].sin_port = htons(PORT[i]);

		sfd[i] = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(sfd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
		bind(sfd[i], (struct sockaddr*)&address[i], sizeof(address[i]));
		listen(sfd[i], 10);
	}

	struct timeval tv;
	fd_set rfds;
	FD_ZERO(&rfds);

	while(1)
	{
		FD_SET(sfd[0], &rfds);
		FD_SET(sfd[1], &rfds);
		FD_SET(sfd[2], &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		int s = select(returnMax(sfd[0], sfd[1], sfd[2]), &rfds, NULL, NULL, &tv);
		int c, nsfd;
		if(FD_ISSET(sfd[0], &rfds))
		{
			nsfd = accept(sfd[0], (struct sockaddr*)&address[0], (socklen_t*)&addrlen);
			c = fork();
			if(c == 0)
			{
				for(int i = 0; i < 2; i++)
					close(sfd[i]);

				dup2(nsfd, 0);
				dup2(nsfd, 1);
				char *args[] = {"./s1", NULL};
				execvp(args[0], args);
				break;
			}			
		}
		if(FD_ISSET(sfd[1], &rfds))
		{
			nsfd = accept(sfd[1], (struct sockaddr*)&address[1], (socklen_t*)&addrlen);
			c = fork();
			if(c == 0)
			{
				for(int i = 0; i < 2; i++)
					close(sfd[i]);

				dup2(nsfd, 0);
				dup2(nsfd, 1);
				char *args[] = {"./s2", NULL};
				execvp(args[0], args);
				break;
			}			
		}
		if(FD_ISSET(sfd[2], &rfds))
		{
			nsfd = accept(sfd[2], (struct sockaddr*)&address[2], (socklen_t*)&addrlen);
			c = fork();
			if(c == 0)
			{
				for(int i = 0; i < 2; i++)
					close(sfd[i]);

				dup2(nsfd, 0);
				dup2(nsfd, 1);
				char *args[] = {"./s3", NULL};
				execvp(args[0], args);
				break;
			}			
		}

		if(c > 0)
			close(nsfd);
	}
}