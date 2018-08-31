#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*
	Read And Write Are Blocking Calls

*/


int main(){
	int status;
	int p[2];
	pid_t pid;
	status = pipe(p);
	char buff[256];
	pid = fork();
	if(pid < 0){
		printf("Fork Error\n");
		exit(0);
	}else if(pid==0){
		//child process code
		printf("Child Process Pid : %d Parent Pid %d\n",getpid(),getppid());
		printf("Enter the Message\n");
		fgets(buff,sizeof(buff),stdin);
		//scanf("%s",buff);
		close(p[0]);//closing read end of pipe
		write(p[1],buff,sizeof(buff));
		close(p[1]);
		printf("Exiting Child Process\n");
		exit(0);
	}else{
		//parent process code
		printf("Waiting For Child To Write in Pipe Pid : %d\n",getpid());
		close(p[1]);//closing write end of pipe
		read(p[0],buff,10);//As pipes are Byte Stream
		sleep(15);
		printf("Child Message Is : %s\n",buff);
		close(p[0]);
		exit(0);
	}
	
	return 0;
}
