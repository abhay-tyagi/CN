#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void main()
{
	int fd = open("/dev/chardev", O_RDONLY);

	char str[12];
	read(fd, str, sizeof(str));

	printf("%s\n", str);
}