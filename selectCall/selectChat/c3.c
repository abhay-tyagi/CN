#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void main()
{
	char self[] = "c3";

	int ffd;
	while((ffd = open("SERVING", O_WRONLY)) < 0);

	write(ffd, self, sizeof(self));

	close(ffd);

	int p = fork();

	if(p > 0)
	{
		while(1)
		{
			int ffd1 = open("SERVING", O_WRONLY);

			char buff[40], content[30];
			fgets(content, sizeof(content), stdin);
			buff[0] = '3';

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
			int ffd2 = open(self, O_RDONLY);

			char buffer[40];
			read(ffd2, buffer, sizeof(buffer));
			printf("%s\n", buffer);

			close(ffd2);
		}
	}
}