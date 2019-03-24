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
	int pp[2];
	pipe(pp);

	int fd = dup(pp[1]);
	dup2(pp[0], 0);

	int chld = fork();

	if(chld > 0)
	{
		char ch[] = "a", buffPar[10];
		while(1)
		{
			sleep(1);
			write(fd, ch, 1);
			printf("Parent wrote %s\n", ch);
			ch[0]++;
			sleep(1);
		}
	}
	else
	{
		char *args[] = {"./pipeexec2", NULL};
		execvp(args[0], args);
	}
}
