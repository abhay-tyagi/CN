#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/select.h>

void main()
{
	char self[] = "c2";

	int ffd;
	while((ffd = open("SERVING", O_WRONLY)) < 0);
	write(ffd, self, sizeof(self));
	close(ffd);

	struct timeval tv;
	fd_set rfds;
	FD_ZERO(&rfds);

	while(1) 
	{
		int ffd2 = open(self, O_RDONLY | O_NONBLOCK); 
		FD_SET(0, &rfds);
		FD_SET(ffd2, &rfds);
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		int i = select(ffd2 + 1, &rfds, NULL, NULL, &tv);
		if(FD_ISSET(ffd2, &rfds))
		{
			char str[50] = {0};
			read(ffd2, str, sizeof(str));
			write(1, str, sizeof(str));		
		}
		else if(FD_ISSET(0, &rfds))
		{
			char buff[40] = {0}, content[30] = {0};
			read(0, content, sizeof(content));
			buff[0] = self[1];

			int i;
			for(i = 0; content[i] != '\0'; i++)
				buff[i+1] = content[i];
			buff[i+1] = '\0';

			int ffd1 = open("SERVING", O_WRONLY);
			write(ffd1, buff, sizeof(buff));
			close(ffd1);			
		}
		close(ffd2);
	}
}