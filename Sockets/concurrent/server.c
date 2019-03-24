#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8080

char* performWork()
{
	return "Did work";
}

void main()
{
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	address.sin_port = htons(PORT);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;

	int opt = 1;
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	bind(sfd, (struct sockaddr*)&address, sizeof(address));
	listen(sfd, 10);

	while(1)
	{
		int nsfd = accept(sfd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

		if(fork() == 0)
		{
			close(sfd);
			while(1)
			{
				char buff[1024] = {0};
				recv(nsfd, buff, sizeof(buff), 0);
				printf("Received %s\n", buff);

				char* response = performWork();
				send(nsfd, response, strlen(response), 0);
			}
			close(nsfd);
		}
		else
			close(nsfd);
	}
}