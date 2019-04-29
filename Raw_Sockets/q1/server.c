#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h> 
#include <time.h>
#include <fcntl.h>

#define PORT 8080 
#define SERVICE_PORT 9090

void main() 
{ 
	int opt = 1;  	
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	struct sockaddr_in address, cli_addr;
	int addrlen = sizeof(cli_addr);
	address.sin_family = AF_INET; 
	if(inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0) 
		printf("\nInvalid address / Address not supported \n"); 
	address.sin_port = htons(PORT); 
	
	bind(sfd, (struct sockaddr*)&address, sizeof(address));
	listen(sfd, 10);

	while(1)
	{
		int nsfd = accept(sfd, (struct sockaddr*)&cli_addr, (socklen_t*)&addrlen); 

		if(fork() == 0)
		{
			close(sfd);

		    char buffer[1024] = {0}, recv_ip[20] = {0}, file[20] = {0};
			recv(nsfd, buffer, sizeof(buffer), 0);
			printf("%s\n", buffer);

			int i = 0, c = 0;
			for(; buffer[i] != '#'; i++)
				recv_ip[i] = buffer[i];
			i++;
			for(; i < strlen(buffer); i++)
				file[c++] = buffer[i];

			int rsfd = socket(AF_INET, SOCK_RAW, 20);
			int fd = open(file, O_RDONLY);
			struct sockaddr_in service_addr;
			service_addr.sin_family = AF_INET;
			service_addr.sin_port = htons(SERVICE_PORT);
			service_addr.sin_addr.s_addr = inet_addr(recv_ip);

			connect(rsfd, (struct sockaddr*)&service_addr, sizeof(service_addr));
			printf("Connected to service\n");

			dup2(rsfd, 1);
			dup2(fd, 0);

			char* args[] = {"./s2", NULL};
			execvp(args[0], args);
		}
		else
			close(nsfd);
	}
}