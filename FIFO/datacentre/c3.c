#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

void main()
{
	if(fork() > 0)
	{
		while(1)
		{
			int ffd;
			while(ffd = open("SERVING", O_WRONLY) < 0);

			char buff[3];
			buff[0] = '3';
			buff[1] = '1';
			buff[2] = '\0';

			write(ffd, buff, sizeof(buff));
			close(ffd);

			sleep(2);
		}
	}
	else
	{
		while(1)
		{
			int ffd = open("c3", O_RDONLY);

			char buff[30];
			read(ffd, buff, sizeof(buff));
			close(ffd);

			printf("Received %s\n", buff);
		}
	}

}