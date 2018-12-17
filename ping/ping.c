#include <unistd.h>           // close()
#include <string.h>           // strcpy, memset(), and memcpy()
#include <netdb.h>            // struct addrinfo
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t
#include <sys/socket.h>       // needed for socket()
#include <netinet/in.h>       // IPPROTO_RAW, IPPROTO_IP, IPPROTO_ICMP, INET_ADDRSTRLEN
#include <netinet/ip.h>       // struct ip and IP_MAXPACKET (which is 65535)
#include <netinet/ip_icmp.h>  // struct icmp, ICMP_ECHO
#include <arpa/inet.h>        // inet_pton() and inet_ntop()
#include <sys/ioctl.h>        // macro ioctl is defined
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/if.h>           // struct ifreq
#include <time.h>
#include <sys/time.h>
#include <sys/cdefs.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <errno.h>            // errno, perror()
#include <pthread.h>
#include <math.h>
#include <byteswap.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/udp.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// Define some constants.
#define IP4_HDRLEN 20         // IPv4 header length
#define ICMP_HDRLEN 8         // ICMP header length for echo request, excludes data
#define checkError(eno,msg)               \
  ({                                      \
          if((eno)<0)                       \
          {                               \
                  perror(msg);            \
                  exit(EXIT_FAILURE);     \
         }                                \
 })
#define BUFSIZE 1500
#define srcport 8000
struct sockaddr_in recvAddress;
struct sockaddr_in address;
int udp_client_fd;
int raw_socket;
char IP[20];
char recvIP[20];
socklen_t salen;
char recvbuf[BUFSIZE];
char sendbuf[BUFSIZE];
pid_t pid;
int datalen = 56;
int nsent;
int udpSocket;
struct hostent *hname;

unsigned short in_cksum(unsigned short *addr, int len){
	int				nleft = len;
	int				sum = 0;
	unsigned short	*w = addr;
	unsigned short	answer = 0;

	/*
	 * Our algorithm is simple, using a 32 bit accumulator (sum), we add
	 * sequential 16 bit words to it, and at the end, fold back all the
	 * carry bits from the top 16 bits into the lower 16 bits.
	 */
	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

		/* 4mop up an odd byte, if necessary */
	if (nleft == 1) {
		*(unsigned char *)(&answer) = *(unsigned char *)w ;
		sum += answer;
	}

		/* 4add back carry outs from top 16 bits to low 16 bits */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = ~sum;				/* truncate to 16 bits */
	return(answer);
}

void tv_sub(struct timeval *out, struct timeval *in){
	if ( (out->tv_usec -= in->tv_usec) < 0) {	/* out -= in */
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}

void sendICMP(){
	int len;
	
	struct icmp *icmp;
	icmp = (struct icmp *)sendbuf;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_id = pid;
	icmp->icmp_seq = nsent++;
	memset(icmp->icmp_data,0xa5,datalen);
	gettimeofday((struct timeval *)icmp->icmp_data,NULL);

	len = 8 + datalen;
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = in_cksum((u_short *)icmp,len);
	//printf("IP is %s\n",IP);
	inet_pton(AF_INET,IP,&address.sin_addr.s_addr);
	//perror("inet_pton");
	address.sin_family = AF_INET;
	//address.sin_port = 22;
	sendto(raw_socket,sendbuf,len,0,(struct sockaddr *)&address,sizeof(address));
	//perror("Send ICMP Packet");
}

void sig_alrm(int signo){
	sendICMP();
	alarm(1);
	return;
}

void processICMP(){
	int size;
	char controlbuf[BUFSIZE];
	ssize_t n;
	struct timeval tval;
	if ((raw_socket = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    	perror ("socket() failed to get socket descriptor for using ioctl() ");
    	exit (EXIT_FAILURE);
  	}
  	//setuid(getuid);
  	size = 60*1024;
  	setsockopt(raw_socket, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
  	//perror("setsockopt");
	sig_alrm(SIGALRM);
	recvAddress.sin_family = AF_INET;
	recvAddress.sin_port = 0;
	for( ; ; ){
		int ana = sizeof(recvAddress);
		n = recvfrom(raw_socket,recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&recvAddress,&ana);
		//perror("recvmsg");
		if(n < 0){
			if(errno == EINTR)
				continue;
			else
				printf("recvmsg error\n");
		}
		gettimeofday(&tval,NULL);
		int hlen1,icmplen;
		double rtt;
		struct ip *ip;
		struct icmp *icmp;
		struct timeval *tvsend;
		ip = (struct ip *)recvbuf;
		hlen1 = ip->ip_hl << 2;
		if(ip->ip_p != IPPROTO_ICMP){
			printf("Received Packet Is Not ICMP\n");
		}
		inet_ntop(AF_INET, &recvAddress.sin_addr, recvIP, INET_ADDRSTRLEN);
		icmp = (struct icmp *)(recvbuf + hlen1);
		if((icmplen = n - hlen1) < 8){
			printf("Malformed Packet\n");
		}
		if(icmp->icmp_type == ICMP_ECHOREPLY){
			if(icmp->icmp_id != pid){
				printf("ICMP Packet Is Not a response to our ECHO_REQUEST\n");
			}
			if(icmplen < 16){
				printf("Not Enough Data To Use\n");
			}
			tvsend = (struct timeval *)icmp->icmp_data;
			tv_sub(&tval,tvsend);
			rtt = tval.tv_sec * 1000.0 + tval.tv_usec / 1000.0;
			printf("%d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms\n",icmplen,recvIP,icmp->icmp_seq,ip->ip_ttl,rtt);
		}else{
			printf("%d bytes from %s: type = %d, code = %d\n",icmplen,recvIP,icmp->icmp_type,icmp->icmp_code);		
		}
	}
}


int main(int argc,char *argv[]){
	if(argc < 2){
			printf("Please provide Ip address\n");
			exit(0);
	}
	memset (&address, 0, sizeof (address));
	memset (&recvAddress, 0, sizeof (recvAddress));
	udp_client_fd = socket (AF_INET, SOCK_DGRAM, 0);
	checkError (udp_client_fd, "UDPSOCKETfailed");
	strcpy(IP,argv[1]);
	pid = getpid() & 0xffff;
	signal(SIGALRM,sig_alrm);
	printf("PING %s : %d data bytes\n",IP,datalen);
	processICMP();

	return 0;
}
