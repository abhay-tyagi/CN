#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>

void main(int argc, char* argv[])
{
	char buff[1024] = {0}, str[100] = {0};
	read(0, buff, sizeof(buff));

	int toScreen = atoi(argv[2]);
	write(toScreen, buff, strlen(buff));

	int fromKeyboard = atoi(argv[1]);
	read(fromKeyboard, str, sizeof(str));

	write(1, str, strlen(str));

	kill(20737, SIGUSR1);
}