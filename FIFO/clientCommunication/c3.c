#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void main()
{
	char req[] = "c4", self[] = "c3";

	int ffd;
	while((ffd = open("SERVING", O_WRONLY)) < 0);

	printf("%s Found the server\n", self);
	write(ffd, req, sizeof(req));
	close(ffd);

	int ffd1;
	while((ffd1 = open(req, O_WRONLY)) < 0);

	printf("%s Connected with %s\n", self, req);

	char s[] = "a";
	while(1)
	{
		sleep(2);
		
		write(ffd1, s, sizeof(s));
		s[0]++;
	}
}