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
	char buff[10];
	while(1)
	{
		read(0, buff, 1);
		printf("pipeexec2 read: %s\n", buff);
	}
}
