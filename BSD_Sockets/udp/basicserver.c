#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 

#define PORT 8080 
#define FPORT 7070

void main() 
{ 
	int opt = 1;  	
	int sfd = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in address, foreign_addr;
	int addrlen = sizeof(foreign_addr);
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(PORT); 
	foreign_addr.sin_family = AF_INET; 
	foreign_addr.sin_addr.s_addr = INADDR_ANY; 
	foreign_addr.sin_port = htons(FPORT); 	

	bind(sfd, (struct sockaddr*)&address, sizeof(address));

    char buffer[1024] = {0}; 
	recvfrom(sfd, buffer, sizeof(buffer), MSG_WAITALL, (struct sockaddr*)&foreign_addr, (socklen_t*)&addrlen); 
	printf("%s\n", buffer); 

	char response[] = "Yeah got you";
	sendto(sfd, response, strlen(response), MSG_CONFIRM, (struct sockaddr*)&foreign_addr, sizeof(foreign_addr)); 
	close(sfd);
} 
