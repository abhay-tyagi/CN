#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <sys/select.h>
#include <fcntl.h>
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h> 
#include <ctype.h>
#include <arpa/inet.h> 
#include <sys/shm.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>

struct mesg_buffer { 
	long mesg_type; 
	char mesg_text[100]; 
}; 

const char *name = "CNMID";
int *mem;
int shm_fd, size = 2 * sizeof(int);

void main()
{
	shm_fd = shm_open(name, O_RDWR, 0666);
	ftruncate(shm_fd, size);
	mem = mmap(NULL, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);

	key_t key = ftok("/tmp", 67);
	int msgid = msgget(key, 0666);

	while(1)
	{
		if(mem[0] == 0)
		{
			struct mesg_buffer message1;
			message1.mesg_type = 1;
			int n = msgrcv(msgid, &message1, sizeof(message1), message1.mesg_type, 0);

			int fd = fileno(popen("./screen", "w"));
			write(fd, message1.mesg_text, strlen(message1.mesg_text));

			if(isdigit(message1.mesg_text[0]))
			{
				printf("zzzz\n");
				kill(mem[1], SIGUSR1);

				struct sockaddr_in serv_addr; 
				memset(&serv_addr, '0', sizeof(serv_addr)); 
				serv_addr.sin_family = AF_INET; 
				serv_addr.sin_port = htons(atoi(message1.mesg_text)); 
				
				if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) 
				{ 
					printf("\nInvalid address / Address not supported \n"); 
					exit(0);
				}

				int sfd = socket(AF_INET, SOCK_STREAM, 0);
				if(connect(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
				{ 
					printf("\nConnection Failed \n"); 
					exit(0);
				}
		
				while(1)
				{
					char buffer[1024] = {0};
					int n = recv(sfd, buffer, sizeof(buffer), 0);

					if(n)
					{
						int fd = fileno(popen("./screen", "w"));
						write(fd, buffer, strlen(buffer));
					}
					else
					{
						close(sfd);
						break;
					}
				}
				kill(mem[1], SIGUSR2);
			}
		}
	}
}