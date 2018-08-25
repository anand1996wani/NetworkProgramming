#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc,char **argv){
	pid_t pid;
	int status;
	pid = fork();
	if(pid < 0){	
		printf("Fork Error\n");
	}else if(pid==0){
		for(int i = 0;i<5;i++){
			printf("Child Process %d\n",i);
		}
	}else{
		//wait(&status);
		waitpid(pid,0,0);//parent will not execute till child terminates
		//waitpid(pid,0,WNOHANG);
		for(int j = 0;j<5;j++){
			printf("Parent Process %d\n",j);
		}
	}
	return 0;
}
