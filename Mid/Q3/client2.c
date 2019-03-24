#include <stdio.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h> 

#define FacPORT 9999
#define FPORT 9090 
#define PORT 6060

int main() 
{ 
	if(fork())
	{
		struct sockaddr_in serv_addr; 
		memset(&serv_addr, '0', sizeof(serv_addr)); 
		serv_addr.sin_family = AF_INET; 
		serv_addr.sin_port = htons(FPORT); 
		
		if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) 
		{ 
			printf("\nInvalid address / Address not supported \n"); 
			return -1; 
		} 

		int sfd = socket(AF_INET, SOCK_STREAM, 0);
		if(connect(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
		{ 
			printf("\nConnection Failed \n"); 
			while(1);
		}
		
		char buffer[1024] = {0};
		recv(sfd, buffer, sizeof(buffer), 0); 
		printf("%s\n",buffer ); 

		close(sfd);

	}
	else
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
		foreign_addr.sin_port = htons(FacPORT); 	

		bind(sfd, (struct sockaddr*)&address, sizeof(address));

		char buffer[] = "Hey Man";
		sendto(sfd, buffer, strlen(buffer), MSG_CONFIRM, (struct sockaddr*)&foreign_addr, sizeof(foreign_addr)); 

		while(1)
		{
		    char response[1024] = {0}; 
			recvfrom(sfd, response, sizeof(response), MSG_WAITALL, (struct sockaddr*)&foreign_addr, (socklen_t*)&addrlen); 
			printf("%s\n", response);
		}
		
		close(sfd);
	}

	return 0; 
} 
