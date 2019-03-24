#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

char groups[][3] = {"g1", "g2", "g3", "g4"};

void main()
{
	char self[] = "c1";

	int p = fork();

	if(p > 0)
	{
		while(1)
		{
			char server[10];
			fgets(server, sizeof(server), stdin);
			printf("Requesting %s\n", server);

			int ffd1 = open(groups[server[1] - '1'], O_WRONLY);

			char buff[40], content[30];
			fgets(content, sizeof(content), stdin);
			buff[0] = '1';

			int i;
			for(i = 0; content[i] != '\0'; i++)
				buff[i+1] = content[i];
			buff[i+1] = '\0';

			write(ffd1, buff, sizeof(buff));

			close(ffd1);
		}
	}
	else
	{
		while(1)
		{
			int ffd2;
			while((ffd2 = open(self, O_RDONLY)) < 0);

			char buffer[40];
			read(ffd2, buffer, sizeof(buffer));
			printf("%s\n", buffer);

			close(ffd2);
		}
	}
}