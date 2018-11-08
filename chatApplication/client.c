#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>

int main(){
	//Step 1 Create A Socket
	//AF_INET denotes Network Socket
	//SOCK_STREAM denotes TCP Socket
	// 0 is the protocol
	int clientSocket;
	clientSocket = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2 Structure for Server address
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(9002);//htons converts int to appropriate data format
	serverAddress.sin_addr.s_addr = INADDR_ANY; //same as 0.0.0.0
	int connectionStatus = connect(clientSocket,(struct sockaddr * )&serverAddress,sizeof(serverAddress));
	if(connectionStatus == -1){
		printf("There Was An Error While Connecting To The Remote Server\n");
		exit(1);
	}else{
		printf("Client Socket Is Created Successfully\n");		
	}
	char clientMessage[255];
	char serverMessage[255];
	
	int childPid;
	if((childPid = fork())==0){
		while(1){
			scanf("%s",&clientMessage);
			if(clientMessage!=NULL){
				send(clientSocket,&clientMessage,sizeof(clientMessage),0);
				bzero(clientMessage,sizeof(clientMessage));
			}
		}
	}else{
		while(1){
			if(recv(clientSocket,&serverMessage,sizeof(serverMessage),0) < 0){
				printf("Error While Receiving The Message\n");
			}else{
				printf("Server : \t%s\n",serverMessage);
				if(strcmp(serverMessage,":exit")==0){
					close(clientSocket);
					break;
					exit(0);
				}	
			}
		}
	}
	return 0;
}
