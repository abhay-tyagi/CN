#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>

void main()
{
	char buff[] = "P1 wrote\n";
	FILE *fd1 = popen("./p2", "r");
	FILE *fd2 = popen("./p3", "r");
	FILE *fd3 = popen("./p4", "r");

	FILE *fdout = popen("./p6", "w");

	struct pollfd p[3];
	p[0].fd = fileno(fd1);
	p[0].events = POLLIN;
	p[1].fd = fileno(fd2);
	p[1].events = POLLIN;
	p[2].fd = fileno(fd3);
	p[2].events = POLLIN;

	while(1)
	{
		int i = poll(p, 3, 0);
		char str[10];
	
		if(i > 0)
		{
			if(p[0].revents == POLLIN)
			{
				fgets(str, 10, fd1);
				fputs(str, fdout);
				fflush(fdout);
			}
			else if(p[1].revents == POLLIN)
			{
				fgets(str, 10, fd2);
				fputs(str, fdout);
				fflush(fdout);
			}
			else if(p[2].revents == POLLIN)
			{
				fgets(str, 10, fd3);
				fputs(str, fdout);
				fflush(fdout);
			}
		}
		else
		{
			fputs(buff, fdout);
			fflush(fdout);

			sleep(2);
		}
	}
}