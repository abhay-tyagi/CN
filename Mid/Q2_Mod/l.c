#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <time.h>

#define PORT 8080 

void main() 
{ 
	int opt = 1;  	
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	struct sockaddr_in address;
	int addrlen = sizeof(address);
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(PORT); 
	
	bind(sfd, (struct sockaddr*)&address, sizeof(address));
	listen(sfd, 10);

	while(1)
	{
		int nsfd = accept(sfd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
		for(int i = 0; i < 5; i++)
		{
			char response[] = "Hey there"; 
			send(nsfd, response, strlen(response), 0); 
			sleep(1);

		}
		close(nsfd);
	}
} 
