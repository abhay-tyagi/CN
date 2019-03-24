#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h> 
#include <string.h>

struct mesg_buffer
{
	long mesg_type;
	char mesg_text[100];
};

void main()
{
	key_t key1 = ftok("/tmp", 65);
	int msgid = msgget(key1, 0666 | IPC_CREAT);

	if(fork() > 0)
	{
		while(1)
		{
			struct mesg_buffer message;

			message.mesg_type = 1;
			fgets(message.mesg_text, sizeof(message.mesg_text), stdin);
			msgsnd(msgid, &message, sizeof(message), 0);
		}
	}
	else
	{
		while(1)
		{
			struct mesg_buffer message;

			message.mesg_type = 2;
			msgrcv(msgid, &message, sizeof(message), message.mesg_type, 0);
			printf("%s\n", message.mesg_text);
		}
	}
}