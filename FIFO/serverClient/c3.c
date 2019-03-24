#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>

void main()
{
	printf("c3 is %i\n", getpid());
	srand(time(0));

	mkfifo("SERVING", 0666);
	int ffd = open("SERVING", O_WRONLY);

	char my_pid[10];
	sprintf(my_pid, "%i", getpid());
	while(1)
	{
		sleep(2);

		char req[] = "a";
		write(ffd, my_pid, sizeof(my_pid));

		int ffd1 = open(my_pid, O_WRONLY), num = rand() % 3;
		req[0] += num;

		write(ffd1, req, sizeof(req));
	}
}