#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void main()
{
	int ffd1;
	while((ffd1 = open("c4", O_RDONLY)) < 0);

	while(1)
	{
		char s[1];
		read(ffd1, s, sizeof(s));
		write(0, s, sizeof(s));
	}
}