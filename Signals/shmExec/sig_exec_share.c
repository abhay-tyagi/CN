#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

const char *name = "CN3";
int *mem;
int shm_fd, size = 2 * sizeof(int);
pid_t p;

void from_P2()
{
	if(mem[1])
	{
		mem[0] = mem[1] + 1;
		printf("X became%i\n", mem[0]);
	}
	else
		printf("No\n");

	sleep(1);
	kill(p, SIGUSR1);
	sleep(1);
}

void main()
{
	signal(SIGUSR2, from_P2);

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, size);
	mem = (int*) mmap(NULL, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	mem[0] = 1;

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
		char *args[] = {"./sig_exec_share2", NULL};
		execvp(args[0], args);
	}
}