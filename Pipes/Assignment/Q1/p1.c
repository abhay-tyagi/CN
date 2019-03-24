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
	// int fd = fileno(popen("./p2", "w"));	
	// dup2(fd, 1);

	char str[] = "a"; 
	while(1)
	{
		sleep(1);
		write(1, str, 1);  //Writing
		str[0]++;
		sleep(1);
	}
}