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

#define PORT 10001

int sfd, opt = 1, addrlen;
struct sockaddr_in address;

void main(int argc, char* argv[])
{
	printf("%i\n", getpid());

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	addrlen = sizeof(address);

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	bind(sfd, (struct sockaddr*)&address, sizeof(address));
	listen(sfd, 10);

	while(1)
	{
		printf("Handling\n");
		char response[] = "Hi. This is a response from s1";

		int nsfd = accept(sfd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		send(nsfd, response, strlen(response), 0); 
		close(nsfd);	
	}
}