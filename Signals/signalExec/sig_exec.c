#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

pid_t p;

void from_P2()
{
	printf("%s\n", "Received signal from sig_exec2");
	sleep(1);
	kill(p, SIGUSR1);
	sleep(1);
}

void main()
{
	signal(SIGUSR2, from_P2);

	p = fork();

	if(p > 0)
	{
		sleep(1);
		kill(p, SIGUSR1);
		sleep(1);
		while(1);
	}
	else if(p == 0)
	{	
		char *args[] = {"./sig_exec2", NULL};
		execvp(args[0], args);
	}
}