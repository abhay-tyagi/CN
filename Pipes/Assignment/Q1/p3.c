#include <stdio.h>
#include <unistd.h>

void main()
{
	char s[1];

	while(1)
	{
		// sleep(2);
		read(0, s, 1);
		printf("P3 Read: %s\n", s);
	}
}