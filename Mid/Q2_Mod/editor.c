#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>

struct mesg_buffer { 
	long mesg_type; 
	char mesg_text[100]; 
} message1; 

const char *name = "CNMID";
pid_t p;
int *mem;
int shm_fd, size = 2 * sizeof(int);

void incCount()
{
	printf("Increased count\n");
	mem[0]++;
}

void decCount()
{
	printf("Decreased count\n");
	mem[0]--;
}

int giveMax(int fd[2])
{
	if(fd[1] >= fd[0])
		return fd[1];
	else
		return fd[0];
}

void main()
{
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, size);
	mem = mmap(NULL, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	mem[0] = 0;
	mem[1] = getpid();

	signal(SIGUSR2, decCount);
	signal(SIGUSR1, incCount);

	key_t key = ftok("/tmp", 67);
	int msgid = msgget(key, 0666 | IPC_CREAT);

	int fd[2], turn = 1, sz = 2;
	fd[0] = open("R1", O_RDONLY | O_NONBLOCK);
	fd[1] = open("R2", O_RDONLY | O_NONBLOCK);

	fd_set rfds;
	FD_ZERO(&rfds);

	while(1)
	{
		for(int i = 0; i < sz; i++)
			FD_SET(fd[i], &rfds);

		int ind = select(giveMax(fd) + 1, &rfds, NULL, NULL, NULL);

		for(int i = 0; i < sz; i++)
		{
			if(FD_ISSET(fd[i], &rfds))
			{
				char buff[100] = {0};
				read(fd[i], buff, sizeof(buff));

				if(buff[0] == '/' && buff[1] == 'd')
				{
					if(fork() == 0)
					{
						dup2(fd[i], 0);
						char* args[] = {"./docWriter", NULL};

						execvp(args[0], args);
					}
				}

				message1.mesg_type = turn;
				strcpy(message1.mesg_text, buff);

				msgsnd(msgid, &message1, sizeof(message1), 0);
				turn = 3 - turn; 
			}
		}
	}
}