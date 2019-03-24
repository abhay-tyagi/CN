#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
	
void from_P2()
{
	printf("%s\n", "Received signal from sig_unrelated2");
}

void main()
{
	signal(SIGUSR2, from_P2);

	const char *name = "CN1";

	int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666), size = 2 * sizeof(int);
	ftruncate(shm_fd, size);
	int *mem = (int*) mmap(NULL, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	mem[0] = getpid();

	while(1)
	{
		sleep(1);
		if(mem[1])
			kill(mem[1], SIGUSR1);
		sleep(1);
	}

}