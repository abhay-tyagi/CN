#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

void main()
{
	mkfifo("SERVING", 0666);
	int ffd = open("SERVING", O_RDONLY);

	while(1)
	{
		char req[3];
		read(ffd, req, sizeof(req));

		mkfifo(req, 0666);
	}
}