#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

char groups[][3] = {"g1", "g2", "g3", "g4"};
char fd[10][10];
int l = 0, servers = 4;

void* manageChat(void* args)
{
	char* serv = (char*) args;
	char name[] = "c0";
	char members[10][10];

	printf("Server %s started.\n", serv);

	int num = 0;
	while(1)
	{
		char req[30];

		int ffd = open(serv, O_RDONLY);
		read(ffd, req, sizeof(req));
		close(ffd);

		if(req[1] == 'j')
		{	
			char buff[3];
			buff[0] = 'c';
			buff[1] = req[0];
			buff[2] = '\0';

			strcpy(members[num], buff);
			mkfifo(buff, 0666);
			num++;

			printf("Added %s to %s\n", buff, serv);
		}
		else
		{
			name[1] = req[0];

			char content[60];
			content[0] = 'c';
			content[1] = req[0];
			content[2] = '(';
			content[3] = 'g';
			content[4] = serv[1];
			content[5] = ')';
			content[6] = ':';
			content[7] = ' ';

			int i, flag = 0;
			for(i = 1; req[i] != '\0'; i++)
				content[i+7] = req[i];
			content[i+7] = '\0';

			for(i = 0; i < num; i++)
			{
				printf("%s\n", members[i]);
				printf("%s\n", name);
				if(strcmp(members[i], name) == 0)
				{
					flag = 1;
					break;
				}
			}

			if(!flag)
			{
				printf("Not a member. Send j to join. ONLY ONCE.\n");
				continue;
			}

			for(int i = 0; i < num; i++)
			{
				if(strcmp(members[i], name))
				{
					int ffd1 = open(members[i], O_WRONLY);
					write(ffd1, content, sizeof(content));
					close(ffd1);
				}
			}
		}
	}
}

void main()
{
	char name[] = "c0";

	for(int i = 0; i < servers; ++i)
		mkfifo(groups[i], 0666);

	pthread_t w[servers];
	for(int i = 0; i < servers; i++)
		pthread_create(&w[i], NULL, manageChat, groups[i]);

	for(int i = 0; i < servers; i++)
		pthread_join(w[i], NULL);
}