#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

void from_P1()
{
	printf("%s\n", "Received signal from parent");
}

void from_P2()
{
	printf("%s\n", "Received signal from child");
}

void main()
{
	signal(SIGUSR1, from_P1);
	signal(SIGUSR2, from_P2);

	pid_t par = getpid();
	pid_t p = fork();

	if(p > 0)
	{
		sleep(1);
		kill(p, SIGUSR1);
		sleep(1);
	}
	else if(p == 0)
	{
		sleep(1);
		kill(par, SIGUSR2);
		sleep(1);
	}
}