#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<errno.h>

#define BUF_LEN 100
#define PORT 8121

int main(int argc, char** argv)
{
	int rsfd=socket(AF_INET,SOCK_RAW,atoi(argv[1]));
	struct sockaddr_in servaddr;
	servaddr.sin_family    = AF_INET; // IPv4 
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.9"); 
        servaddr.sin_port = htons(PORT); 
	bind(rsfd, (const struct sockaddr *)&servaddr,sizeof(servaddr));
	
	int server_fd,new_socket;
	struct sockaddr_in address;
	int addrlen;
	addrlen = sizeof(address);
	server_fd = socket(AF_INET, SOCK_STREAM , 0) ; 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = inet_addr("127.0.0.4"); 
	address.sin_port = htons( PORT );
	bind(server_fd, (struct sockaddr *)&address,addrlen);
	listen(server_fd, 25);
	fd_set rfds;
	struct timeval tv;
    	int retval;
    	FD_ZERO(&rfds);    	
    	
    	while(1)
    	{
    		tv.tv_sec=1;
		tv.tv_usec=0;
		FD_SET(server_fd,&rfds);
		FD_SET(rsfd,&rfds);
		int maxi=server_fd;
		if(maxi<rsfd)
			maxi=rsfd;
			
		retval=select(maxi+1,&rfds,NULL,NULL,&tv);
    		if(retval>0)
    		{
    			if(FD_ISSET(server_fd,&rfds))
    			{
    				new_socket=accept(server_fd,(struct sockaddr *)&address, (socklen_t*)&addrlen);
    				pid_t p1;
    				p1=fork();
    				if(p1>0)
    				{
    					close(new_socket);
    					continue;
    				}
    				close(server_fd);
    				while(1)
		 		{
		 			char msg[]="TakeThisFromS1", buffer[100];

		 			int br=read(new_socket,buffer,100);
		 			write(new_socket,msg,strlen(msg)+1);
		 		}
    			}
    			if(FD_ISSET(rsfd,&rfds))
    			{
    				printf("\n");
    				char buf[BUF_LEN];
				struct sockaddr_in client;
				socklen_t clilen=sizeof(client);
				recvfrom(rsfd,buf,BUF_LEN,0,(struct sockaddr*)&client,( socklen_t*)&clilen);
				char str[1000];
				inet_ntop(AF_INET, &(client.sin_addr), str, 1000);
				printf("ip is %s\n",str);
				struct iphdr *ip;
			  	ip=(struct iphdr*)buf;
				printf("%s\n", (buf+(ip->ihl)*4) );
				char buff[]="8121Service1";	
				sendto(rsfd,buff,strlen(buff)+1,0,(struct sockaddr*)&client,sizeof(client));
				//while(1);
    			}
    		}	
			
    	}
}
