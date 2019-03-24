#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>

const char *name = "CN";
int *mem;
int shm_fd, size = 2 * sizeof(int);
pid_t p;

struct mesg_buffer {
	long mesg_type;
	char mesg_text[100];
};

int myGrp = 0;
#define numGrps 5

void main()
{
	key_t key[numGrps], mainKey;
	int msgid[numGrps], mainId;

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, size);
	mem = (int*) mmap(NULL, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	mem[0] = myGrp;

	for(int i = 0; i < numGrps; i++)
	{
		key[i] = ftok("/tmp", 65 + i);
		msgid[i] = msgget(key[i], 0666 | IPC_CREAT);
	}

	mainKey = ftok("/etc", 80);
	mainId = msgget(mainKey, 0666 | IPC_CREAT);

	int p = fork();
	printf("%i\n", p);
	if(p > 0)
	{
		struct mesg_buffer message;
		while(1)
		{
			while(mem[0] == 0);
			msgrcv(msgid[mem[0]], &message, sizeof(message), myGrp, 0);
			printf("%s\n", message.mesg_text);
		}	
	}
	else if(p == 0)
	{
		while(1)
		{
			char buff[100], grp[10];
			fgets(grp, sizeof(grp), stdin);
			printf("Requesting group %s\n", grp);

			fgets(buff, sizeof(buff), stdin);

			if(buff[0] == 'j')
			{
				myGrp = grp[1] - '1';
				mem[0] = myGrp;
			}

			struct mesg_buffer message;

			message.mesg_type = grp[1] - '0';
			message.mesg_text[0] = '1';

			int i;
			for(i = 1; buff[i] != '\0'; i++)
				message.mesg_text[i] = buff[i - 1];
			message.mesg_text[i] = '\0';

			msgsnd(mainId, &message, sizeof(message), 0);
		}
	}
}