#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(){
	//char message[255];
	int serverSocket;
	//creating server socket
	serverSocket = socket(AF_INET,SOCK_STREAM,0);
	
	//define address structure
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(9002);
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	bind(serverSocket,(struct sockaddr *)&serverAddress,sizeof(serverAddress));
	listen(serverSocket,5);//5 is the number of connections possible
	
	//printf("Enter the message for sending to the client\n");
	//scanf("%s",message);
	//send the message
	char serverMessage[255];
	char clientMessage[255];
	int clientSocket = accept(serverSocket,NULL,NULL);
	if(clientSocket < 0){
		printf("Error conecting the client\n");
	}
	else{
		printf("Connected With The Client Successfully\n");
	}
	int childPid;
	if((childPid = fork())==0){
		while(1){
			scanf("%s",&serverMessage);
			if(serverMessage!=NULL){
				//printf("Server : \t");
				send(clientSocket,&serverMessage,sizeof(serverMessage),0);
				//send(clientSocket,&serverMessage,5,0);
				bzero(serverMessage,sizeof(serverMessage));
			}
		}
	}else{
		while(1){
			//printf("Client : \t");
			if(recv(clientSocket,&clientMessage,sizeof(clientMessage),0) < 0){
				printf("Error While Receiving The Message\n");
			}else{
				printf("Client : \t%s\n",clientMessage);
				if(strcmp(clientMessage,":exit")==0){
					close(serverSocket);
					break;
					exit(0);
				}	
			}
		}
	}
	return 0;
}

