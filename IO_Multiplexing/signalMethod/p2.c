#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

const char* name = "Cn";
int *mem;

void sema_wait()
{
	printf("p2wait\n");
	while(*mem == 0){printf("%mem in p2 is%i\n", *mem);}
	*mem--;
}

void sema_signal()
{
	*mem++;
}

void main()
{
	sleep(2);
	int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, sizeof(int));

	mem = (int*) mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0);

	char ch[] = "P2 wrote\n";
	while(1)
	{
		sleep(1);

		sema_wait();
		write(1, ch, sizeof(ch));
		kill(getppid(), SIGUSR1);
		sleep(1);
		sema_signal();
	}
}