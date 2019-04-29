#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

char *socket_path = "socket";

void main() 
{
	struct sockaddr_un addr, cli_addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, socket_path);
	unlink(socket_path);

	int usfd = socket(AF_UNIX, SOCK_STREAM, 0), addrlen = sizeof(cli_addr);
	bind(usfd, (struct sockaddr*)&addr, sizeof(addr));
	listen(usfd, 5);

	int nsfd = accept(usfd, (struct sockaddr*)&cli_addr, (socklen_t*)&addrlen);
	while (1) 
	{
		// Read
		char buf[100] = {0};
		int n = recv(nsfd, buf, sizeof(buf), 0);
		if(n == 0)
		{
			printf("Closed\n");
			break;
		}

		printf("Received: %s\n", buf);

		// Write
		char msg[100] = {0};
		read(0, msg, sizeof(msg));
		send(nsfd, msg, sizeof(msg), 0);
	}
}