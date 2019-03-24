#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

pid_t p;
int* mem;
const char* name = "CN10";
int shm_fd, size = 3 * sizeof(int);

void from_P1()
{
	if(mem[0])
	{
		mem[1] = mem[0] + 1;

		printf("Y became %i\n", mem[1]);
	}
}

void main()
{
	signal(SIGUSR1, from_P1);

	p = getpid();
	printf("P2 is %i\n", p);

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, size);
	mem = mmap(NULL, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	mem[1] = 1;
	// mem[2] = p;

	while(1)
	{
		while(!mem[2] || mem[2] == p);

		printf("P2 calling%i\n", mem[2]);

		sleep(1);
		kill(mem[2], SIGUSR2);
		sleep(1);
		mem[2] = p;
	}
}