#include <unistd.h>
#include <time.h>
#include <string.h>

void main()
{
	char buff[] = "S3 Responded";
	for(int i = 0; i < 2; i++)
	{
		write(1, buff, strlen(buff));
		sleep(2);
	}
}