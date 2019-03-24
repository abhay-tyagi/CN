#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

void main()
{
	int fd[2];
	pipe(fd);

	int chld = fork();
	char buff[10];

	if(chld > 0)
	{
		close(fd[0]);

		char ch[] = "a";
		while(1)
		{
			write(fd[1], ch, 1);
			printf("I wrote\n");
			sleep(1);
		}
	}
	else
	{
		close(fd[1]);

		while(1)
		{
			printf("I read: \n");
			sleep(2);
			read(fd[0], buff, 1);
			printf("%s", buff);
		}
	}
}