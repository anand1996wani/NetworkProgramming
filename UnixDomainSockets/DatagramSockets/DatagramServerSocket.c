#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h> 

#define BUF_SIZE 100


int main(int argc,char *argv[]){
	struct sockaddr_un serverAddress,clientAddress;
	int serverSocketFd;
	ssize_t numRead;
	char buffer[BUF_SIZE];
	serverSocketFd = socket(AF_UNIX,SOCK_DGRAM,0);
	if(serverSocketFd==-1){
		printf("Error Creating Socket\n");
		exit(0);
	}
	memset(&serverAddress,0,sizeof(struct sockaddr_un));
	serverAddress.sun_family = AF_UNIX;
	strncpy(serverAddress.sun_path,argv[1],sizeof(serverAddress.sun_path)-1);
	if(bind(serverSocketFd,(struct sockaddr *)&serverAddress,sizeof(struct sockaddr_un))==-1){
		printf("Bind Error\n");
		exit(0);
	}
	int len = sizeof(struct sockaddr_un);
	numRead = recvfrom(serverSocketFd,buffer,BUF_SIZE,0,(struct sockaddr *)&clientAddress,&len);
	if(numRead==-1){
		printf("Data Recv Error\n");
	}
	printf("Data Read From Client Path %s Is : %s \n",clientAddress.sun_path,buffer);
	//printf("Enter Reply For The Client\n");
	//scanf("%s",buffer);
	//sendto(serverSocketFd,buffer,sizeof(buffer),0,(struct sockaddr *)&clientAddress,sizeof(clientAddress));
	close(serverSocketFd);
	unlink(argv[1]);
	return 0;
}
