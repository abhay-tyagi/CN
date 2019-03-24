#include <stdlib.h>
#include <string.h>
#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <sys/types.h>
#include <errno.h>

struct mesg_buffer { 
	long mesg_type; 
	char mesg_text[100]; 
} message1, message2; 

void main() 
{ 
	key_t key = ftok("/tmp", 65);
	int msgid = msgget(key, 0666 | IPC_CREAT);

	message1.mesg_type = 1;
	strcpy(message1.mesg_text, "HeyLLo");
	for(int i = 0; i < 5; i++)
		msgsnd(msgid, &message1, sizeof(message1), 0);

	struct msqid_ds ds;
	msgctl(msgid, IPC_STAT, &ds);

	printf("Here are the queue details: \n");
	printf("Time of last message received: %ld\n", ds.msg_rtime);
	printf("Time of last message sent: %ld\n", ds.msg_stime);
	printf("Time of last change: %ld\n", ds.msg_ctime);
	printf("Number of messages in queue: %ld\n", ds.msg_qnum);
	printf("Maximum bytes allowed in queue: %ld\n", ds.msg_qbytes);
	printf("PID of process that last sent: %i\n", ds.msg_lspid);
	printf("PID of process that last received: %i\n", ds.msg_lrpid);

	msgctl(msgid, IPC_RMID, NULL);
} 
