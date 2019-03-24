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
	char s[] = "P3 wrote\n";
	while(1)
	{
		sleep(1);
		fputs(s, stdout);
		fflush(stdout);
		sleep(2);
	}
}