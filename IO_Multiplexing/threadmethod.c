#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

int pp[2];
sem_t sem, rdsem;

void* P1(void* args)
{
	printf("Entered P1\n");
	int fd1 = dup(pp[1]);

	char str[] = "P1 wrote\n";
	while(1)
	{
		sem_wait(&sem);
		sem_wait(&rdsem);
		write(fd1, str, sizeof(str));
		printf("%s\n", str);

		sem_post(&sem);
		sleep(2);
	}
}

void* P2(void* args)
{
	printf("Entered P2\n");
	int fd2 = dup(pp[1]);

	char str[] = "P2 wrote\n";
	while(1)
	{
		sem_wait(&sem);
		sem_wait(&rdsem);
		write(fd2, str, sizeof(str));
		printf("%s\n", str);

		sem_post(&sem);
		sleep(2);
	}
}

void* P3(void* args)
{
	printf("Entered P3\n");
	int fd3 = dup(pp[1]);

	char str[] = "P3 wrote\n";
	while(1)
	{
		sem_wait(&sem);
		sem_wait(&rdsem);
		write(fd3, str, sizeof(str));
		printf("%s\n", str);

		sem_post(&sem);
		sleep(2);
	}
}

void main()
{
	pipe(pp);
	dup2(pp[0], 0);

	sem_init(&sem, 1, 1);
	sem_init(&rdsem, 1, 1);

	pthread_t w1, w2, w3;

	pthread_create(&w2, NULL, P2, NULL);
	pthread_create(&w3, NULL, P3, NULL);
	pthread_create(&w1, NULL, P1, NULL);

	sleep(2);

	char buff[20];
	while(1)
	{
		sleep(1);
		read(0, buff, 20);
		printf("Read: %s\n", buff);

		sem_post(&rdsem);
	}

	pthread_join(w1, NULL);
	pthread_join(w2, NULL);
	pthread_join(w3, NULL);
}