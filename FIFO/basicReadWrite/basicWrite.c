#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h> 

void main()
{
	mkfifo("ABC", 0666);

	int ffd = open("ABC", O_WRONLY);
	dup2(ffd, 1);

	char s[] = "a\n";
	while(1)
	{
		sleep(1);
		write(1, s, 2);
		s[0]++;
		sleep(1);
	}
}