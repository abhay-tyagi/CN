#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void main()
{
	while(1)
	{
		char ser[] = "Service of d1";
		write(1, ser, sizeof(ser));

		sleep(2);
	}
}