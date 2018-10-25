#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#define BUF_SIZE 100
#define BACKLOG 5

#define CONTROLLEN CMSG_LEN(sizeof(int))

/*

TCP Server but it will not accept client connections. It will receive connected client File Discriptor from the other process using UNIX Domain sockets. This program just accepts client fd using recvmsg of UNIX Domain sockets.  

*/

int recvFd(int sockfd){
	
	static struct cmsghdr *cmptr = NULL;
	struct iovec iov[1];
	struct msghdr msg;
	char buf[2];
	iov[0].iov_base = buf;
	iov[0].iov_len = 2;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	cmptr = malloc(CONTROLLEN);
	msg.msg_control = cmptr;
	msg.msg_controllen = CONTROLLEN;
	recvmsg(sockfd,&msg,0);
	if(buf[1] < 0){
		printf("Failed\n");
		exit(0);
	}
	cmptr = CMSG_FIRSTHDR(&msg);
	while(cmptr!=NULL){
		if(cmptr->cmsg_level==SOL_SOCKET && cmptr->cmsg_type==SCM_RIGHTS){
			return *(int *) CMSG_DATA(cmptr);
		}
		cmptr = CMSG_NXTHDR(&msg,cmptr);
	}
}

int main(int argc,char *argv[]){
	char message[255];
	int serverSocket;
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
	
	int clientSocket = recvFd(clientSocketFd);
	
	
	printf("Enter the message for sending to the client\n");
	scanf("%s",message);
	//send the message
	send(clientSocket,&message,sizeof(message),0);
	
	
	return 0;
}

