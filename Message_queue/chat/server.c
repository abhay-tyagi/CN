#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

struct mesg_buffer { 
	long mesg_type; 
	char mesg_text[100]; 
} message1, message2; 

char fd[10][10];
int l = 0;

void main() 
{ 
	key_t key = ftok("/tmp", 65); 
	key_t chatkey = ftok("/etc", 66);
	int msgid = msgget(key, 0666 | IPC_CREAT);
	int msgid2 = msgget(chatkey, 0666);

	while(1)
	{	
		msgrcv(msgid, &message1, sizeof(message1), 0, 0);

		if(message1.mesg_text[0] == 'c' && isdigit(message1.mesg_text[1]))
		{
			strcpy(fd[l], message1.mesg_text);
			l++;

			printf("%s is here. Total %i participants.\n", message1.mesg_text, l);
		}
		else
		{	
			char buff[100];
			buff[0] = 'c';
			buff[1] = message1.mesg_text[0];
			buff[2] = ':';
			buff[3] = ' ';

			int i;
			for(i = 1; message1.mesg_text[i] != '\0'; i++)
				buff[i+3] = message1.mesg_text[i];
			buff[i+3] = '\0';

			strcpy(message1.mesg_text, buff);

			for(int i = 0; i < l; i++)
			{
				if(buff[1] - '0' != i+1)
				{
					message1.mesg_type = i + 1;
					msgsnd(msgid2, &message1, sizeof(message1), 0);
				}
			}
		}
	}

	msgctl(msgid, IPC_RMID, NULL);
	msgctl(msgid2, IPC_RMID, NULL);
} 
