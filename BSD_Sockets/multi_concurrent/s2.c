#include <unistd.h>
#include <time.h>
#include <string.h>

void main()
{
	char buff[] = "S2 Responded";
	while(1)
	{
		char msg[100] = {0};
		read(0, msg, sizeof(msg));
		write(1, buff, strlen(buff));
	}
}