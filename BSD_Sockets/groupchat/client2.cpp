#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define PORT 8080
#define MYPORT 7070

char self[] = "c2";
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

	struct sockaddr_in my_addr; 
	memset(&my_addr, '0', sizeof(my_addr)); 
	my_addr.sin_family = AF_INET; 
	my_addr.sin_port = htons(MYPORT); 
	my_addr.sin_addr.s_addr = INADDR_ANY;
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(sfd, (struct sockaddr*)&my_addr, sizeof(my_addr));
	
	if(connect(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	}

	if(fork() > 0)
	{
		while(1)
		{
			int i;
			char str[90], msg[100];
			scanf("%s", str);
			msg[0] = self[1];
			for(i = 1; i <= strlen(str); i++)
				msg[i] = str[i-1];
			msg[i] = '\0';

			send(sfd, msg, strlen(msg), 0); 
		}
	}
	else
	{
		while(1)
		{
			char buffer[1024] = {0};
			int n = recv(sfd, buffer, sizeof(buffer), 0); 
			if(n == 0)
				break;
			printf("%s\n", buffer); 
		}
	}

	close(sfd);
}