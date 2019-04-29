#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void main()
{
	printf("Started\n");
	char buf;
	while((buf = getchar()) != EOF)
		printf("%c\n", buf);
}