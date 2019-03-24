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
	char s[] = "P6 received: ", str[10];
	while(1)
	{
		fgets(str, 10, stdin);

		fputs(s, stdout);
		fputs(str, stdout);

		fflush(stdout);
	}
}