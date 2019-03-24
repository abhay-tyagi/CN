#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

int fd1, fd2;

void from_P1()
{
	char buff[20];
	
	sleep(1);
	read(fd1, buff, 20);
	printf("Read: %s\n", buff);
}

void from_P2()
{
	char buff[20];

	sleep(1);
	read(fd2, buff, 20);
	printf("Read: %s\n", buff);
}

void main()
{
	signal(SIGUSR1, from_P1);
	signal(SIGUSR2, from_P2);

	fd1 = fileno(popen("./p1", "r"));
	fd2 = fileno(popen("./p2", "r"));

	int c1 = fork();

	if(c1 > 0)
	{
		sleep(1);
		int c2 = fork();

		if(c2 > 0)
		{
			sleep(1);
			while(1);
		}
		else
		{
			char* args[] = {"./p2", NULL};
			execvp(args[0], args);
		}
	}
	else
	{
		char* args[] = {"./p1", NULL};
		execvp(args[0], args);
	}
}