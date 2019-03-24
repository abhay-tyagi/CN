#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

char clients[3][10];
int l[3] = {0};

int servers[3] = {0};

int already_exists(char sender[3], int s)
{
	for(int i = 0; i < strlen(clients[s]); i++)
		if(sender[1] == clients[s][i])
			return 1;

	return 0;
}

void main()
{
	mkfifo("SERVING", 0666);

	while(1)
	{
		int ffd = open("SERVING", O_RDONLY);

		char buff[30];
		read(ffd, buff, sizeof(buff));

		char sender[3];
		sender[0] = 'c';
		sender[1] = buff[0];
		sender[2] = '\0';

		char service[5];
		service[0] = '.';
		service[1] = '/';
		service[2] = 'd';
		service[3] = buff[1];
		service[4] = '\0';

		if(servers[buff[1] - '1'] == 0)
		{
			servers[buff[1] - '1'] = fileno(popen(service, "r"));
			printf("Started service %c\n", buff[1]);
		}

		if(!already_exists(sender, buff[1] - '1'))
		{
			clients[l[buff[1] - '1']][0] = sender[1];
			l[buff[1] - '1']++;

			printf("Added client to list\n");
		}

		mkfifo(sender, 0666);

		for(int i = 0; i < 3; i++)
		{
			char content[30];

			if(servers[i])
			{
				read(servers[i], content, sizeof(content));
				for(int j = 0; j < strlen(clients[i]); j++)
				{
					char sender[3];
					sender[0] = 'c';
					sender[1] = clients[i][j];
					sender[2] = '\0';

					int ffd1 = open(sender, O_WRONLY);
					write(ffd1, content, sizeof(content));
					close(ffd1);
				}
			}
		}

		close(ffd);
	}
}