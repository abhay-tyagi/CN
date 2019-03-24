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
	int fd1[2], fd2[2];
	pipe(fd1);
	pipe(fd2);

	int chld = fork();

	if(chld > 0)
	{
		close(fd1[0]);
		close(fd2[1]);

		char ch[] = "a", buffPar[10];
		while(1)
		{
			write(fd1[1], ch, 1);
			printf("Parent wrote %s\n", ch);
			ch[0]++;
			sleep(1);

			read(fd2[0], buffPar, 1);
			printf("Parent read%s\n", buffPar);
			sleep(1);
		}
	}
	else
	{
		sleep(2);

		close(fd1[1]);
		close(fd2[0]);

		char ch[] = "z", buffChld[10];
		while(1)
		{
			write(fd2[1], ch, 1);
			printf("Child wrote %s\n", ch);
			ch[0]--;
			sleep(1);

			read(fd1[0], buffChld, 1);
			printf("Child read: %s\n", buffChld);
			sleep(1);
		}
	}
}