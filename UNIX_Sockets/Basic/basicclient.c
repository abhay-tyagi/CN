#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *socket_path = "socket";

void main() 
{
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, socket_path);

	int usfd = socket(AF_UNIX, SOCK_STREAM, 0);
	connect(usfd, (struct sockaddr*)&addr, sizeof(addr));

	while(1)
	{
		// Write
		char msg[100] = {0};
		read(0, msg, sizeof(msg));
		send(usfd, msg, sizeof(msg), 0);

		// Read
		char buf[100] = {0};
		int n = recv(usfd, buf, sizeof(buf), 0);
		if(n == 0)
		{
			printf("Closed\n");
			break;
		}

		printf("Received: %s\n", buf);
	}
}
