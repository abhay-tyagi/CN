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
	int fd = fileno(popen("./pipeexec2", "w"));
	int chld = fork();

	if(chld > 0)
	{
		char ch[] = "a";
		while(1)
		{
			sleep(1);
			write(fd, ch, 1);
			// printf("Parent wrote %s\n", ch);
			ch[0]++;
			sleep(1);
		}
	}
	else
	{
		// char *args[] = {"./pipeexec2", NULL};
		// execvp(args[0], args);
	}
}
