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
	char ch[] = "a";
	while(1)
	{
		sleep(1);
		write(1, ch, 1);
		ch[0]++;
		sleep(1);
	}
}