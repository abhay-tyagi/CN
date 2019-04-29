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

void* func(void* param)
{
	int* p = (int*)(param);
	int sfd= *p;
	char buffer[20]="";
	while(1)
	{
		read(0,buffer,100);
		write(sfd,buffer,strlen(buffer)+1);
		read(sfd,buffer,100);
		printf("Got String as : %s\n",buffer);
	}
}
//need to use sudo for raw socket to work
int main(int argc, char** argv)
{
	int server_fd;
	struct sockaddr_in serv_addr;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.4");
	
	int rsfd=socket(AF_INET,SOCK_RAW,atoi(argv[1]));
	struct sockaddr_in myaddr;
	myaddr.sin_family    = AF_INET; // IPv4 
        myaddr.sin_addr.s_addr = inet_addr("127.0.0.3"); 
        myaddr.sin_port = htons(8122); 
	bind(rsfd, (const struct sockaddr *)&myaddr,sizeof(myaddr));
	int optval=1;
	setsockopt(rsfd, IPPROTO_IP, SO_BROADCAST, &optval, sizeof(int));//IP_HDRINCL
	struct sockaddr_in client;
	client.sin_family=AF_INET;
	client.sin_addr.s_addr=inet_addr("127.0.0.2");
	char buff[]="Request";	
	unsigned int client_len=sizeof(client);
	sendto(rsfd,buff,strlen(buff)+1,0,(struct sockaddr*)&client,sizeof(client));
	
	fd_set rfds;
	struct timeval tv;
    	int retval;
    	FD_ZERO(&rfds);
    	int coun=0;
    	
    	while(1)
    	{
    		tv.tv_sec=1;
		tv.tv_usec=0;
		FD_SET(rsfd,&rfds);
		int maxi=-1;
		if(maxi<rsfd)
			maxi=rsfd;
			
		retval=select(maxi+1,&rfds,NULL,NULL,&tv);
    		if(retval>0)
    		{
    			if(FD_ISSET(rsfd,&rfds))
    			{
    				char buf[1024];
				struct sockaddr_in client;
				socklen_t clilen=sizeof(client);
				recvfrom(rsfd,buf,1024,0,(struct sockaddr*)&client,( socklen_t*)&clilen);
				struct iphdr *ip;
			  	ip=(struct iphdr*)buf;
			  	char buf1[100]="";
				printf("Tried to connect to %s\n", (buf+(ip->ihl)*4) );
				strcpy(buf1,(buf+(ip->ihl)*4));
				char buf2[100]="";
				strncpy(buf2,buf1,4);
				printf("%s\n",buf2);
				if(atoi(buf2)==atoi(argv[2]) && coun==0)
				{
					serv_addr.sin_port = htons(atoi(buf2));
					connect(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); 
					pthread_t p;
					pthread_create(&p,NULL,func,(void*)(&server_fd));
					coun=1;
				}
    			}
    		}	
    	}
}
