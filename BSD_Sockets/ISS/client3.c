#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 10003

int main()
{
	struct sockaddr_in address, foreign_addr;
	int addrlen = sizeof(foreign_addr);
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(PORT); 

    memset(&foreign_addr, 0, sizeof(foreign_addr)); 
	foreign_addr.sin_family = AF_INET; 
	foreign_addr.sin_addr.s_addr = INADDR_ANY; 
	foreign_addr.sin_port = htons(PORT); 	
	
	int sfd = socket(AF_INET, SOCK_DGRAM, 0);
	// bind(sfd, (struct sockaddr*)&address, sizeof(address));

	char msg[] = "Hi\n";
	sendto(sfd, msg, strlen(msg), MSG_CONFIRM, (struct sockaddr *) &foreign_addr, sizeof(foreign_addr)); 

    char response[1024] = {0}; 
	recvfrom(sfd, response, sizeof(response), MSG_WAITALL, (struct sockaddr*)&foreign_addr, (socklen_t*)&addrlen); 
	printf("Received: %s\n", response); 

	close(sfd);
}