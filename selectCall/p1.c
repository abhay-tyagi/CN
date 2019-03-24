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
	char s[] = "P1 wrote\n";
	while(1)
	{
		sleep(1);
		char bff[20];
		fgets(bff, sizeof(bff), stdin);
		fputs(bff, stdout);
		fflush(stdout);
		sleep(1);
	}
}
