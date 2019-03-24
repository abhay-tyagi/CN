#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char fd[10][10];
int l = 0;

void main()
{
	mkfifo("SERVING", 0666);

	while(1)
	{
		char req[40] = {0};
		int ffd = open("SERVING", O_RDONLY);
		read(ffd, req, sizeof(req));
		close(ffd);
 
		if(req[0] == 'c')
		{
 			mkfifo(req, 0666);
			strcpy(fd[req[1] - '1'], req);
			l++;
		}
		else
		{
			char content[50] = {0};
			content[0] = 'c';
			content[1] = req[0];
			content[2] = ':';
			content[3] = ' ';

			int j;
			for(j = 1; req[j] != '\0'; j++)
				content[j+3] = req[j];
			content[j+3] = '\0';

			for(int i = 0; i < l; i++)
			{
				if(req[0] - '1' != i)
				{
					int ffd1 = open(fd[i], O_WRONLY);
					write(ffd1, content, sizeof(content));
					close(ffd1);

					// printf("Wrote %s to %s\n", content, fd[i]);
				}
			}
		}
	}
}