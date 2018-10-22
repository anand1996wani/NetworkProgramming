#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/un.h>

#define BUF_SIZE 100
#define BACKLOG 5

int main(int argc,char *argv[]){
	struct sockaddr_un serverAddress;
	int clientSocketFd;
	char buffer[BUF_SIZE];
	clientSocketFd = socket(AF_UNIX,SOCK_STREAM,0);
	if(clientSocketFd==-1){
		printf("Error Creating Socket\n");
		exit(0);
	}
	memset(&serverAddress,0,sizeof(struct sockaddr_un));
	serverAddress.sun_family = AF_UNIX;
	strcpy(serverAddress.sun_path,argv[1]);
	if(connect(clientSocketFd,(struct sockaddr *)&serverAddress,sizeof(struct sockaddr_un))==-1){
		printf("Connection Error\n");
		exit(0);
	}
	printf("Enter Data  : \n");
	scanf("%s",buffer);
	write(clientSocketFd,buffer,sizeof(buffer));
	close(clientSocketFd);
	return 0;
}
