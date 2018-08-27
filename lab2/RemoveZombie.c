#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

/*

Write an exit handler such that it clears all the zombies of this process.

*/

void exitHandler(){
	while(wait(NULL)!=-1){
		wait(NULL);
	}
	exit(0);
}

int main(){
	pid_t pid1,pid2;
	pid1 = fork();
	signal(SIGINT,exitHandler);
	if(pid1 < 0){
		printf("Fork Error\n");
	}else if(pid1==0){
		//Child Process
		printf("In Child : pid = %d\n Parent pid := %d\n",getpid(),getppid());
		printf("Child Finishing\n");
		exit(0);
	}else{
		//Parent Process
		//wait(NULL);//Removing this wil create make child process zombie
		pid2 = fork();
		if(pid2==0){	
			printf("In Child : pid = %d\n Parent pid := %d\n",getpid(),getppid());
			exit(0);	
		}else{
			while(1);
			exit(0);
		}
	}
	
	return 0;
}
