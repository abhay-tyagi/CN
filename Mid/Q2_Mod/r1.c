#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h>

void main()
{
	mkfifo("R1", 0666);
	int ffd = open("R1", O_WRONLY);

	while(1)
	{
		char buff[100] = {0};
		read(0, buff, sizeof(buff));
		write(ffd, buff, strlen(buff));
	}

	close(ffd);
}