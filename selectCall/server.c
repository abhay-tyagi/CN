#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>

void main()
{
	char buff[] = "P1 wrote\n";
	FILE *fd1 = popen("./p1", "r");
	FILE *fd2 = popen("./p2", "r");
	FILE *fd3 = popen("./p3", "r");

	struct timeval tv;
	fd_set rfds;
	FD_ZERO(&rfds);

	while(1)
	{
		FD_SET(fileno(fd1), &rfds);
		FD_SET(fileno(fd2), &rfds);
		FD_SET(fileno(fd3), &rfds);

		tv.tv_sec = 0;
		tv.tv_usec = 0;

		int i = select(6, &rfds, NULL, NULL, &tv);
		char str[10];
	
		if(FD_ISSET(fileno(fd1), &rfds))
		{
			fgets(str, 10, fd1);
			fputs(str, stdout);
			fflush(stdout);
		}
		if(FD_ISSET(fileno(fd2), &rfds))
		{
			fgets(str, 10, fd2);
			fputs(str, stdout);
			fflush(stdout);
		}
		if(FD_ISSET(fileno(fd3), &rfds))
		{
			fgets(str, 10, fd3);
			fputs(str, stdout);
			fflush(stdout);
		}
	}
}