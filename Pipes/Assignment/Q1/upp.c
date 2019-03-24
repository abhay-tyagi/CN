#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

void main(int argc, char* argv[])
{
	if(argc > 2)
	{
		int fd[argc - 2];
		fd[0] = fileno(popen(argv[1], "r"));
		for(int i = 1; i < argc - 1; i++)
		{
			fd[i] = fileno(popen(argv[i+1]))

			if(fork() > 0)
			{
				sleep(1);


			}
			else
			{
				char* args[] = {argv[i], NULL};
				execvp(args[0], args);
			}
		}
	}
	else
	{
		printf("Improper usage\n");
	}



	int fd1 = fileno(popen("./p2", "w"));	
	int fd2 = fileno(popen("./p3", "w")); 
	// dup2(fd, 1);

	char str[] = "a"; 
	while(1)
	{
		sleep(1);
		write(1, str, 1);  //Writing
		str[0]++;
		sleep(1);
	}
}