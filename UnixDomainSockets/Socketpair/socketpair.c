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
	
	pid_t pid;
	char buffer[BUF_SIZE];
	int sockfd[2];
	int ret = socketpair(AF_UNIX,SOCK_STREAM,0,sockfd);
	if(ret == -1){
		printf("SOCKET PAIR ERROR\n");
	}
	pid = fork();
	if(pid==0){
		//child pair
		printf("Enter The Data For Parent \n");
		scanf("%s",buffer);
		write(sockfd[1],buffer,sizeof(buffer));
		exit(0);
	}else{
		//parent process
		read(sockfd[0],buffer,sizeof(buffer));
		printf("Data From Child Is %s\n",buffer);
		exit(0);
	}
	
	return 0;
}
