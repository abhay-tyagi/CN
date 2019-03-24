#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void main()
{
	while(1)
	{
		char buff[100] = {0};
		read(0, buff, sizeof(buff));

		FILE* fd = fopen("writtenNews.txt", "a");
		fputs(buff, fd);
		fclose(fd);
	}
}