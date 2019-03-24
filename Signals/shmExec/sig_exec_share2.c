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

void from_P1()
{
	if(mem[0])
	{
		mem[1] = mem[0] + 1;
		printf("Y became%i\n", mem[1]);
	}
	else
		printf("%s%i\n", "Y", mem[1]);
	sleep(2);
	kill(getppid(), SIGUSR2);
	sleep(2);
}

void main()
{
	signal(SIGUSR1, from_P1);

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, size);
	mem = (int*) mmap(NULL, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	mem[1] = 1;

	while(1);
}