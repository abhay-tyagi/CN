#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

char ser1[] = "a", ser2[] = "b", ser3[] = "c";

void s1(char req[])
{
	printf("Provided s1 to %s\n", req);
}

void s2(char req[])
{
	printf("Provided s2 to %s\n", req);
}

void s3(char req[])
{
	printf("Provided s3 to %s\n", req);
}

void main()
{
	mkfifo("SERVING", 0666);
	int ffd = open("SERVING", O_RDONLY);

	while(1)
	{
		char req[10];
		read(ffd, req, sizeof(req));
		printf("Received request from %s\n", req);

		mkfifo(req, 0666);
		int ffd1 = open(req, O_RDONLY);

		char serv[2];
		read(ffd1, serv, sizeof(serv));

		if(serv[0] == ser1[0])
			s1(req);
		else if(serv[0] == ser2[0])
			s2(req);
		else if(serv[0] == ser3[0])
			s3(req);
		else
			printf("Invalid request\n");

		close(ffd1);
	}
}