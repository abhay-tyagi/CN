#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h> 
#include <time.h>
#include<netinet/ip.h>

#define PORT 9090

void main() 
{ 
	int opt = 1;  	
	int rsfd = socket(AF_INET, SOCK_RAW, 20);
	setsockopt(rsfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	struct sockaddr_in address, cli_addr;
	int addrlen = sizeof(cli_addr);
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = inet_addr("127.0.0.3");
	address.sin_port = htons(PORT); 	
	bind(rsfd, (struct sockaddr*)&address, sizeof(address));

	while(1)
	{
		char buf[1024] = {0};
		recvfrom(rsfd, buf, sizeof(buf), 0, (struct sockaddr*)&cli_addr, (socklen_t*)&addrlen);
		
		char buf1[100];
		struct iphdr *ip;
	  	ip=(struct iphdr*)buf;
		strcpy(buf1,(buf+(ip->ihl)*4));
	
		printf("%s", (buf+(ip->ihl)*4));
	}
} 
