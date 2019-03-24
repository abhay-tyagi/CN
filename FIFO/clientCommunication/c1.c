#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void main()
{
	char req[] = "c2", self[] = "c1";

	int ffd;
	while((ffd = open("SERVING", O_WRONLY)) < 0);

	printf("%s Found the server\n", self);
	write(ffd, req, sizeof(req));
	close(ffd);

	int ffd1;
	while((ffd1 = open(req, O_RDONLY)) < 0);

	printf("Connected with %s\n", req);

	while(1)
	{
		char s[1];
		read(ffd1, s, sizeof(s));
		write(0, s, sizeof(s));
	}
}