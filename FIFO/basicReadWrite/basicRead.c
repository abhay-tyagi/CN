#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h> 

void main()
{
	mkfifo("ABC", 0666);

	int ffd = open("ABC", O_RDONLY);
	dup2(ffd, 0);

	char s[2];
	while(1)
	{
		read(0, s, sizeof(s));
		printf("I read: %s\n", s);
	}
}