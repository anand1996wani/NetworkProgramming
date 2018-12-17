#include "header.h"
#include <time.h>

struct clientStructure{
	int clientFd;
	int flag;
	int client;
};

int flag = 0;
int portNo;
struct clientStructure client[FD_SETSIZE];
fd_set rset,allset;
int clientIndex = -1;
extern int errno;

void exitClient(int i){
	close(client[i].clientFd);
	FD_CLR(client[i].clientFd,&allset);
	client[i].clientFd = -1;
	client[i].client = -1;
	client[i].flag = -1;
}

int main(int argc,char *argv[]){
	if(argc < 2){
		printf("Enter the port no as a command line argument\n");
		exit(0);
	}
	
	portNo = atoi(argv[1]);
	int maxFd = -1;
	int listenFd;	
	
	
	struct sockaddr_in serverAddress,clientAddress;
	listenFd = socket(AF_INET,SOCK_STREAM,0);
	perror("Server Socket");
	if(listenFd == -1){
		printf("Error Creating Server Socket\n");
	}
	
	bzero(&serverAddress,sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(portNo);
	
	bind(listenFd,(struct sockaddr *)&serverAddress,sizeof(serverAddress));
	perror("bind");
	
	listen(listenFd,5);
	perror("listen");
	
	int len = sizeof(clientAddress);
	
	for(int i = 0;i < FD_SETSIZE;i++){
		client[i].clientFd = -1;
		client[i].flag = -1;
		client[i].client = -1;
	}
	
	FD_ZERO(&allset);
	FD_ZERO(&rset);
	
	FD_SET(listenFd,&allset);
	if(listenFd > maxFd){
		maxFd = listenFd; 
	}
	
	int ready = -1;
	
	struct timeval time;
	time.tv_sec = 10;
	time.tv_usec = 0;
	int check = 0;
	int anand = 0;
	int temp = 0;
	for( ; ; ){
		rset = allset;
		if(temp==1){
			ready = select(maxFd + 1,&rset,NULL,NULL,&time);
			perror("select");
			if(time.tv_sec == 0)
				check = 0;
			else
				check = 1;
			printf("Time is %ld\n",time.tv_sec);
		}else{
			ready = select(maxFd + 1,&rset,NULL,NULL,NULL);
		}
		if(FD_ISSET(listenFd,&rset)){
			int clientFd = -1;
			clientFd = accept(listenFd,(struct sockaddr *)&clientAddress,&len);
			perror("accept");
			temp = 1;
			/*int fd_flags = fcntl(clientFd,F_GETFL);
			if(-1==fcntl(clientFd,F_SETFL,fd_flags | O_NONBLOCK)){
				printf("Error While FD Blocking\n");
			}*/
			
			//perror("fcntl");
			int i = -1;
			for(i = 0;i < FD_SETSIZE;i++){
				if(client[i].clientFd < 0){
					client[i].clientFd = clientFd;
					break;
				}
			}
			if(i==FD_SETSIZE){
				printf("Too Many Clients\n");
				exit(0);
			}
			FD_SET(clientFd,&allset);
			if(clientFd > maxFd){
				maxFd = clientFd;
			}
			if(i > clientIndex){
				clientIndex = i;
				printf("Client Connected with index %d\n",i);
			}
			if(--ready <= 0){
				continue;
			}
		}
		int sockFd;
		char buffer[100];
		int n = -1;
		for(int i=0;i <= clientIndex;i++){
			if((sockFd = client[i].clientFd) < 0){
				continue;
			}
			if(FD_ISSET(sockFd,&rset)){
				n = recv(sockFd,buffer,sizeof(buffer),0);
				perror("recv");
				if(n==0){
					printf("Closing Client Connection Received Fin \n");
					exitClient(i);
				}else{
					printf("Data Received From Client Is : %s\n",buffer);//strncmp
					if(buffer[0]=='H' && buffer[1]=='E' && buffer[2]=='L' && buffer[3]=='L' && buffer[4]=='O'){ 
						client[i].flag = 1;
					}else{
						for(int j = 0;j <= clientIndex;j++){
							if(client[j].clientFd !=-1 && client[j].clientFd!=sockFd){
								send(client[j].clientFd,buffer,n,0);
								perror("send");
							}
						}
						bzero(buffer,sizeof(buffer));
					}
				}
			}
		}
		printf("Client Index is %d\n",clientIndex);
		
		//send(client[0].clientFd,"HELLO",strlen("HELLO"),0);
		perror("Send");
		if(check == 0){
			time.tv_sec = 10;
			printf("Timely checking\n");
			if(flag==0){
				printf("flag is %d\n",flag);
				flag = 1;
				for(int i = 0;i <= clientIndex;i++){
					if(client[i].clientFd!=-1){
						printf("flag is %d\n",flag);
						client[i].flag = 0;//HeartBeat Message Hello Is Sent To Client 
						send(client[i].clientFd,"REPLY WITH HELLO MESSAGE",strlen("REPLY WITH HELLO MESSAGE"),0);
						printf("HeartBeat Message Sent To Client %d\n",i);
						client[i].client==1;
						anand = 1;
					}
				}
			}else{
				flag = 0;
				if(anand==1){
					for(int i = 0;i <= clientIndex;i++){
						if(client[i].clientFd!=-1){
							if(client[i].flag!=1 && client[i].client==1){
								printf("HeartBeat Reply Not Received From Client %d \n",i);
								exitClient(i);
							}else{
								printf("HeartBeat Message Received From Client %d \n",i);
							}
						}
					}
				}
			}
		}else{
			printf("Not Time Out\n");
		}
	}
	return 0;
}
