#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(int argc,char *argv[]){
	
	int pipefd0[2];
	int pipefd1[2];
	int pipefd2[2];
	
	pid_t pid1,pid2;
	

	int flag = 0;
	/*char data[100];
	printf("Enter Data \n");
	scanf("%s",data);*/
	char buff[2];
	pipe(pipefd2);
	pid1 = fork();
	if(pid1==0){
		//child process
		printf("Child Process %d\n",getpid());
		fflush(stdout);
		write(pipefd2[1],"1",1);		
		exit(0);
	}else{
		//parent process
		read(pipefd2[0],buff,1);
		buff[1] = '\0';
		printf("Parent Process %d\n",getpid());
		//write(pipefd0[1],buff,1);
		exit(0);	
	}
	
	return 0;
}
