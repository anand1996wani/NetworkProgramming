#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

/*
Printing Alternately In Child And Parent Process

*/

pid_t pid;
int i = 2,j = 1;
void signalHandlerChild(int signal);
void signalHandlerParent(int signal);
void signalHandlerChild(int signal){
	printf("Child : %d\n",i);
	i = i + 2;
	if(i < 5000)
		kill(getppid(),SIGUSR2);		
}


void signalHandlerParent(int signal){
	printf("Parent : %d\n",j);
	j = j + 2;
	if(j < 5000)
		kill(pid,SIGUSR1);	
}

int main(){
	pid = fork();
	signal(SIGUSR2,signalHandlerParent);
	signal(SIGUSR1,signalHandlerChild);
	if(pid < 0){
		printf("Fork Error\n");
	}else if(pid==0){
		//child process
		printf("In Child Process\n");
		kill(getppid(),SIGUSR2);
		while(j < 5000);
	}else{
		//parent process
		printf("In Parent Process\n");
		//kill(pid,SIGUSR1);
		while(i<5000);
	}
	return 0;
}
