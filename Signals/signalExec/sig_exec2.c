#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void from_P1()
{
	printf("%s\n", "Received signal from sig_exec");
	sleep(1);
	kill(getppid(), SIGUSR2);
	sleep(1);
}

void main()
{
	signal(SIGUSR1, from_P1);

	while(1);
}