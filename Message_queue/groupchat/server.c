#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

struct mesg_buffer {
	long mesg_type;
	char mesg_text[100];
};

#define numGrps 5
struct groups {
	char members[10][10];
	int l;

} grps[numGrps];


int isMember(char sender[], int i)
{
	for(int j = 0; j < sizeof(grps[i - 1].members); j++)
		if(strcmp(sender, grps[i - 1].members[j]) == 0)
			return 1;

	return -1;
}

void main()
{
	key_t key[numGrps], mainKey;
	int msgid[numGrps], mainId;

	for(int i = 0; i < numGrps; i++)
	{
		key[i] = ftok("/tmp", 65 + i);
		msgid[i] = msgget(key[i], 0666 | IPC_CREAT);
	}

	mainKey = ftok("/etc", 80);
	mainId = msgget(mainKey, 0666 | IPC_CREAT);

	for(int i = 0; i < numGrps; i++)
		grps[i].l = 0;


	while(1)
	{
		struct mesg_buffer message;
		msgrcv(mainId, &message, sizeof(message), 0, 0);

		char sender[3];
		sender[0] = 'c';
		sender[1] = message.mesg_text[0];
		sender[2] = '\0';

		if(message.mesg_text[1] == 'j')
		{
			if(isMember(sender, message.mesg_type) == -1)
			{
				strcpy(grps[message.mesg_type - 1].members[grps[message.mesg_type - 1].l], sender);
				grps[message.mesg_type - 1].l++;

				printf("Added %s to g%ld\n", sender, message.mesg_type);
			}			
		}
		else
		{
			if(isMember(sender, message.mesg_type) == -1)
			{
				printf("Not a member. Send j to join.\n");
				continue;
			}

			int grp =  message.mesg_type - 1;

			char buff[100];
			buff[0] = 'c';
			buff[1] = sender[0];
			buff[2] = '(';
			buff[3] = 'g';
			buff[4] = message.mesg_type + '0';
			buff[5] = ')';
			buff[6] = ':';
			buff[7] = ' ';

			int i;
			for(i = 1; message.mesg_text[i] != '\0'; i++)
				buff[i + 7] = message.mesg_text[i];
			buff[i + 7] = '\0';

			strcpy(message.mesg_text, buff);
			for(int i = 0; i < grps[grp].l; i++)
			{
				int memNum = grps[grp].members[i][1] - 48;

				if(memNum != sender[0] - '0')
				{
					message.mesg_type = memNum;
					msgsnd(msgid[grp], &message, sizeof(message), 0);

					printf("Sent %s to %i as type %ld\n", message.mesg_text, grp, message.mesg_type);
				}
			}
		}
	}
}