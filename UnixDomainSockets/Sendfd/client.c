#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

//Address Structure 
//specifies where to connect to

//Simple TCP Client Program

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
	}else{
		char serverResponce[255];
		recv(clientSocket,&serverResponce,sizeof(serverResponce),0);//0 is for flags
		printf("The Data Received From The Server Is %s\n",serverResponce);
	}
	//close(clientSocket);
	return 0;
}
