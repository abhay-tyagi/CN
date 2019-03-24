#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void main()
{
	char buff[100] = {0};
	read(0, buff, sizeof(buff));
	write(1, buff, strlen(buff));
}