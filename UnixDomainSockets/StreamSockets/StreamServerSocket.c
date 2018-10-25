#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h> 

#define BUF_SIZE 100
#define BACKLOG 5

int main(int argc,char *argv[]){
	struct sockaddr_un serverAddress;
	int serverSocketFd,clientSocketFd;
	ssize_t numRead;
	char buffer[BUF_SIZE];
	serverSocketFd = socket(AF_UNIX,SOCK_STREAM,0);
	if(serverSocketFd==-1){
		printf("Error Creating Socket\n");
		exit(0);
	}
	memset(&serverAddress,0,sizeof(struct sockaddr_un));
	serverAddress.sun_family = AF_UNIX;
	strcpy(serverAddress.sun_path,argv[1]);
	if(bind(serverSocketFd,(struct sockaddr *)&serverAddress,sizeof(struct sockaddr_un))==-1){
		printf("Bind Error\n");
		exit(0);
	}
	if(listen(serverSocketFd,BACKLOG)==-1){
		printf("Listen Error\n");
		exit(0);
	}
	clientSocketFd = accept(serverSocketFd,NULL,NULL);
	numRead = read(clientSocketFd,buffer,BUF_SIZE);
	printf("Data Read From Client Is : %s \n",buffer);
	printf("Enter Reply For The Client\n");
	scanf("%s",buffer);
	write(clientSocketFd,buffer,sizeof(buffer));
	close(clientSocketFd);
	close(serverSocketFd);
	unlink(argv[1]);
	return 0;
}
