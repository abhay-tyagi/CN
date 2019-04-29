#include <stdio.h>
#include <pcap.h>
#include<netinet/tcp.h>
#include<netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>

#define SIZE_ETHERNET 14

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	const struct sniff_ethernet *ethernet;
	const struct iphdr *ip;
	const struct tcphdr *tcp;

	ethernet = (struct sniff_ethernet*)(packet);

	ip = (struct iphdr*)(packet + SIZE_ETHERNET);
	u_int size_ip = sizeof(struct iphdr);
	
	tcp = (struct tcphdr*)(packet + SIZE_ETHERNET + size_ip);
	u_int size_tcp = sizeof(struct tcphdr);

	const char *payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
	u_int size_payload = ntohs(ip->tot_len) - (size_ip + size_tcp);

	struct sockaddr_in source, dest;
	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = ip->saddr;
	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = ip->daddr;

	printf("Connected System's IP: %s\n", inet_ntoa(dest.sin_addr));
}

int main(int argc, char *argv[])
{
	char *dev, errbuf[PCAP_ERRBUF_SIZE];

	dev = pcap_lookupdev(errbuf);
	if(dev == NULL) 
	{
		fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
		return -1;
	}
	
	printf("Device: %s\n", dev);
	pcap_t *handle = pcap_open_live(dev, BUFSIZ, 0, 1000, errbuf);
	if(handle == NULL) 
	{
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		return -1;
	}

	// if(pcap_datalink(handle) != DLT_EN10MB){fprintf(stderr, "Device %s doesn't provide Ethernet headers - not supported\n", dev);return -1;}
	// struct pcap_pkthdr header;
	// const u_char *packet;

	char filter_exp[] = "ip";
	struct bpf_program fp;
	bpf_u_int32 mask;
	bpf_u_int32 net;

	if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
		    dev, errbuf);
		net = 0;
		mask = 0;
	}

	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) 
	{
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return -1;
	}

	if (pcap_setfilter(handle, &fp) == -1) 
	{
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return -1;
	}

	struct in_addr tmp;
	tmp.s_addr=net;
	printf ("Checking connections to %s\n", inet_ntoa(tmp));

	pcap_loop(handle, 100, got_packet, NULL);
	
	return(0);
}
