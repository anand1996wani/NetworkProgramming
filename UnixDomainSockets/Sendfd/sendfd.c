#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUF_SIZE 100
#define BACKLOG 5

#define CONTROLLEN CMSG_LEN(sizeof(int))

static struct cmsghdr *cmptr = NULL;

int main(int argc,char *argv[]){
	
	
	// TCP Server Code accepting Client Connections
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
	
	
	//TCP Connection Accept
	
	int serverSocket;
	serverSocket = socket(AF_INET,SOCK_STREAM,0);
	
	struct sockaddr_in serverAddress1;
	serverAddress1.sin_family = AF_INET;
	serverAddress1.sin_port = htons(9002);
	serverAddress1.sin_addr.s_addr = INADDR_ANY;
	bind(serverSocket,(struct sockaddr *)&serverAddress1,sizeof(serverAddress1));
	listen(serverSocket,5);//5 is the number of connections possible
	int clientSocket;
	clientSocket = accept(serverSocket,NULL,NULL);
	
	
	
	
	int fd_to_send = clientSocket;
	
	// Code for Sending Connected Client File Discriptor To the client using UNIX Domain Sockets
	
	char buf[2];
	struct iovec iov[1];
	iov[0].iov_base = buf;
	iov[0].iov_len = 2;
	
	struct msghdr msg;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	
	if(fd_to_send < 0){
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		buf[1] = -fd_to_send;
	}else{
		cmptr = malloc(CONTROLLEN);
		cmptr->cmsg_level = SOL_SOCKET;
		cmptr->cmsg_type = SCM_RIGHTS;
		cmptr->cmsg_len = CONTROLLEN;
		msg.msg_iov = iov;
		msg.msg_iovlen = 1;
		msg.msg_control = cmptr;
		msg.msg_controllen = CONTROLLEN;
		*(int *)CMSG_DATA(cmptr) = fd_to_send;
		buf[1] = 0;
	}
	buf[0] = 0;
	if(sendmsg(clientSocketFd,&msg,0)!=2){
		printf("Error Sending\n");
		exit(0);
	}
		
	close(clientSocketFd);
	close(serverSocketFd);
	unlink(argv[1]);
	return 0;
}
