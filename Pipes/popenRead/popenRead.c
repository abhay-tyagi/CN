#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

void main()
{
	int fd = fileno(popen("./popenexec", "r"));
	int chld = fork();

	if(chld > 0)
	{
		sleep(2);
		while(1)
		{
			char buff[10];

			read(fd, buff, 1);
			printf("Parent read: %s\n", buff);				
			sleep(2);
		}
	}
	else
	{
		char *args[] = {"./popenexec", NULL};
		execvp(args[0], args);
	}
}
