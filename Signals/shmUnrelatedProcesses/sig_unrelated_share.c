#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

const char *name = "CN10";
pid_t p;
int *mem;
int shm_fd, size = 3 * sizeof(int);

void from_P2()
{
	if(mem[1])
	{
		mem[0] = mem[1] + 1;

		printf("X became %i\n", mem[0]);
	}
}

void main()
{
	signal(SIGUSR2, from_P2);

	p = getpid();
	printf("P1 is %i\n", p);

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, size);
	mem = mmap(NULL, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	mem[0] = 1;
	mem[2] = p;

	while(1)
	{	
		while(!mem[2] || mem[2] == p);

		printf("P1 calling%i\n", mem[2]);

		sleep(1);
		kill(mem[2], SIGUSR1);
		sleep(1);
		mem[2] = p;
	}
}