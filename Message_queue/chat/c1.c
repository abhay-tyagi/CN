#include <stdio.h> 
#include <sys/ipc.h> 
#include <unistd.h>
#include <string.h>
#include <sys/msg.h> 
#include <errno.h>

struct mesg_buffer { 
	long mesg_type; 
	char mesg_text[100]; 
} message1, message2; 

int main() 
{
	char self[] = "c1";

	key_t key1 = ftok("/tmp", 65); 
	key_t key2 = ftok("/etc", 66);

	int msgid1 = msgget(key1, 0666 | IPC_CREAT); 
	int msgid2 = msgget(key2, 0666 | IPC_CREAT);

	message1.mesg_type = 1;
	strcpy(message1.mesg_text, self);
	msgsnd(msgid1, &message1, sizeof(message1), 0);

	int p = fork();

	if(p > 0)
	{
		while(1)
		{
			char buff[50];

			message1.mesg_type = 1;
			fgets(buff, sizeof(buff), stdin);

			message1.mesg_text[0] = '1';
			int i;
			for(i = 0; buff[i] != '\0'; i++)
				message1.mesg_text[i+1] = buff[i];
			message1.mesg_text[i+1] = '\0';

			msgsnd(msgid1, &message1, sizeof(message1), 0);
		}
	}
	else
	{

		while(1)
		{
			message2.mesg_type = self[1] - '0';

			msgrcv(msgid2, &message2, sizeof(message2), message2.mesg_type, 0);

			printf("%s\n", message2.mesg_text);
		}
	}

	return 0; 
} 
