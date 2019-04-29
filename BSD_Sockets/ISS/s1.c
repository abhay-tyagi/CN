#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

void main()
{
	char buff[] = "S1 Responded\n";
	for(int i = 0; i < 5; i++)
	{
		write(1, buff, strlen(buff));
		sleep(1);
	}
}