#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h> 
#include <time.h>
#define PORT 8080 

int main() 
{ 
	struct sockaddr_in serv_addr; 
	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) 
	{ 
		printf("\nInvalid address / Address not supported \n"); 
		return -1; 
	} 

	while(1)
	{
		int sfd = socket(AF_INET, SOCK_STREAM, 0);
		if(connect(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
		{ 
			printf("\nConnection Failed \n"); 
			return -1; 
		}

		char msg[] = "Hey there";
		send(sfd, msg, strlen(msg), 0); 
		
		char buffer[1024] = {0};
		recv(sfd, buffer, sizeof(buffer), 0); 
		printf("%s\n",buffer ); 

		close(sfd);
		sleep(2);
	}

	return 0; 
} 
