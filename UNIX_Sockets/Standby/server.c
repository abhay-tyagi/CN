#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

char *socket_path = "socket";
#define PORT 8080
#define NUMSOCKS 10

int nsfd[NUMSOCKS] = {0}, l = 0, p = 0;
struct sockaddr_in address, cli_address;
int addrlen = sizeof(address), cli_addrlen = sizeof(cli_address), opt = 1, sfd;
pthread_t w[NUMSOCKS];


int maxAll()
{
	int max = -1;
	for(int i = 0; i < l; i++)
		max = (nsfd[i] >= max) ? nsfd[i] : max;

	return max + 1;
}

int recv_fd(int socket)
{
	int sent_fd, available_ancillary_element_buffer_space;
	struct msghdr socket_message;
	struct iovec io_vector[1];
	struct cmsghdr *control_message = NULL;
	char message_buffer[1];
	char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

	/* start clean */
	memset(&socket_message, 0, sizeof(struct msghdr));
	memset(ancillary_element_buffer, 0, CMSG_SPACE(sizeof(int)));

	/* setup a place to fill in message contents */
	io_vector[0].iov_base = message_buffer;
	io_vector[0].iov_len = 1;
	socket_message.msg_iov = io_vector;
	socket_message.msg_iovlen = 1;

	/* provide space for the ancillary data */
	socket_message.msg_control = ancillary_element_buffer;
	socket_message.msg_controllen = CMSG_SPACE(sizeof(int));

	if(recvmsg(socket, &socket_message, MSG_CMSG_CLOEXEC) < 0)
	return -1;

	if(message_buffer[0] != 'F')
	{
	/* this did not originate from the above function */
	return -1;
	}

	if((socket_message.msg_flags & MSG_CTRUNC) == MSG_CTRUNC)
	{
	/* we did not provide enough space for the ancillary element array */
	return -1;
	}

	/* iterate ancillary elements */
	for(control_message = CMSG_FIRSTHDR(&socket_message);
	   control_message != NULL;
	   control_message = CMSG_NXTHDR(&socket_message, control_message))
	{
	if( (control_message->cmsg_level == SOL_SOCKET) &&
	   (control_message->cmsg_type == SCM_RIGHTS) )
	{
	sent_fd = *((int *) CMSG_DATA(control_message));
	return sent_fd;
	}
	}

	return -1;
}


int send_fd(int socket, int fd_to_send)
 {
  struct msghdr socket_message;
  struct iovec io_vector[1];
  struct cmsghdr *control_message = NULL;
  char message_buffer[1];
  /* storage space needed for an ancillary element with a paylod of length is CMSG_SPACE(sizeof(length)) */
  char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
  int available_ancillary_element_buffer_space;

  /* at least one vector of one byte must be sent */
  message_buffer[0] = 'F';
  io_vector[0].iov_base = message_buffer;
  io_vector[0].iov_len = 1;

  /* initialize socket message */
  memset(&socket_message, 0, sizeof(struct msghdr));
  socket_message.msg_iov = io_vector;
  socket_message.msg_iovlen = 1;

  /* provide space for the ancillary data */
  available_ancillary_element_buffer_space = CMSG_SPACE(sizeof(int));
  memset(ancillary_element_buffer, 0, available_ancillary_element_buffer_space);
  socket_message.msg_control = ancillary_element_buffer;
  socket_message.msg_controllen = available_ancillary_element_buffer_space;

  /* initialize a single ancillary data element for fd passing */
  control_message = CMSG_FIRSTHDR(&socket_message);
  control_message->cmsg_level = SOL_SOCKET;
  control_message->cmsg_type = SCM_RIGHTS;
  control_message->cmsg_len = CMSG_LEN(sizeof(int));
  *((int *) CMSG_DATA(control_message)) = fd_to_send;

  return sendmsg(socket, &socket_message, 0);
 }


int flg = 1, usfd;
struct sockaddr_un backup_addr;
void goForMaintenance()
{
	if(flg)
	{
		usfd = socket(AF_UNIX, SOCK_STREAM, 0);
		connect(usfd, (struct sockaddr*)&backup_addr, sizeof(backup_addr));
		for(int ind = 0; ind < NUMSOCKS; ind++)
		{
			if(nsfd[ind])
			{
				printf("Sending %i\n", nsfd[ind]);
				send_fd(usfd, nsfd[ind]);
				close(nsfd[ind]);
				sleep(1);
			}
		}

		close(usfd);
		close(sfd);

		for(int i = 0; i < p; i++)
			pthread_cancel(w[i]);

		flg = 0;
	}
	else
	{
		usfd = socket(AF_UNIX, SOCK_STREAM, 0);
		connect(usfd, (struct sockaddr*)&backup_addr, sizeof(backup_addr));
		for(int ind = 0; ind < l; ind++)
			nsfd[ind] = 0;

		l = 0;
		p = 0;

		sfd = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
		bind(sfd, (struct sockaddr*)&address, sizeof(address));
		listen(sfd, 10);
		flg = 1;

		char msg[40] = "I am back";
		send(usfd, msg, strlen(msg), 0);
		close(usfd);
	}
}

void* serveIt(void* arg)
{
	char* tmp = (char*) arg;
	int ind = atoi(tmp);

	while(1)
	{
		char msg[30] = "Sent by server";
		send(nsfd[ind], msg, sizeof(msg), 0);
		sleep(1);
	}
}

void main()
{
	signal(SIGTSTP, goForMaintenance);

	backup_addr.sun_family = AF_UNIX;
	unlink(socket_path);
	strcpy(backup_addr.sun_path, socket_path);
	int backup_addrlen = sizeof(backup_addr);

	address.sin_port = htons(PORT);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	bind(sfd, (struct sockaddr*)&address, sizeof(address));
	listen(sfd, 10);

	while(1)
	{
		if(flg)
		{
			printf("Accepting\n");
			nsfd[l] = accept(sfd, (struct sockaddr*)&cli_address, (socklen_t*)&cli_addrlen);
			printf("%i\n", nsfd[l]);

			char ind[10] = {0};
			sprintf(ind, "%d", l);
			l++;
			pthread_create(&w[p++], NULL, serveIt, ind);
			printf("Added nsfd\n");
		}
	}
}