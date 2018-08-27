#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
	pid_t pid;
	pid = fork();
	if(pid < 0){
		printf("Fork Error\n");
	}else if(pid==0){
		//Child Process
		printf("In Child : pid = %d\n Parent pid := %d\n",getpid(),getppid());
		printf("Child Finishing\n");
		exit(0);
	}else{
		//Parent Process
		wait(NULL);//Removing this wil create make child process zombie
		while(1);
		exit(0);
	}
	
	return 0;
}
