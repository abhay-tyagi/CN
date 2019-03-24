#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void main()
{
	int ffd1;
	while((ffd1 = open("c2", O_WRONLY)) < 0);

	char s[] = "z";
	while(1)
	{
		sleep(2);
		
		write(ffd1, s, sizeof(s));
		s[0]--;
	}
}