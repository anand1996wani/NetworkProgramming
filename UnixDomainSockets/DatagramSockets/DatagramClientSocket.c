#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>

#define BUF_SIZE 100

int main(int argc,char *argv[]){
	struct sockaddr_un serverAddress,clientAddress;
	int clientSocketFd;
	char buffer[BUF_SIZE];
	clientSocketFd = socket(AF_UNIX,SOCK_STREAM,0);
	if(clientSocketFd==-1){
		printf("Error Creating Socket\n");
		exit(0);
	}
	memset(&serverAddress,0,sizeof(struct sockaddr_un));
	clientAddress.sun_family = AF_UNIX;
	snprintf(clientAddress.sun_path,sizeof(clientAddress.sun_path),"./clientSocket.%ld",(long)getpid());
	if(bind(clientSocketFd,(struct sockaddr *)&clientAddress,sizeof(struct sockaddr_un))==-1){
		printf("Client Bind Error\n");
		exit(0);
	}
	serverAddress.sun_family = AF_UNIX;
	strncpy(serverAddress.sun_path,argv[1],sizeof(serverAddress.sun_path)-1);
	
	printf("Enter Data  : \n");
	scanf("%s",buffer);
	int ret =  -1;
	ret = sendto(clientSocketFd,buffer,sizeof(buffer),0,(struct sockaddr *)&serverAddress,sizeof(struct sockaddr_un));
	if(ret==-1){
		printf("Client Sending Error\n");
		exit(0);
	}
	//recvfrom(clientSocketFd,buffer,sizeof(buffer),0,NULL,NULL);
	//printf("Server Responce Is : %s \n",buffer);
	close(clientSocketFd);
	return 0;
}
