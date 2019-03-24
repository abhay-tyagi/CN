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
	// int fd1 = fileno(popen("./p3", "w"));
	// dup2(fd1, 1);
	char str[10];
	while(1)
	{
		read(0, str, 1); // Read from stdin
		write(1, str, 1); //Pass on
	}
}