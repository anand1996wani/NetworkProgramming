#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>


int main(){
	int pipefd[2];
	pid_t pid;
	int status;
	status = pipe(pipefd);
	if(status < 0){
		printf("Error Creating The Pipe\n");
		exit(0);
	}
	pid = fork();
	if(pid < 0){
		printf("Fork Error\n");
		exit(0);
	}else if(pid==0){
		//child Process
		close(pipefd[0]);
		close(1);
		dup(pipefd[1]);
		//write(pipefd[1]," anand",6);
		execlp("cat","cat","/etc/passwd",(char *)0);
		//sleep(10);	
	}else{
		//parent Process
		close(pipefd[1]);
		close(0);
		dup(pipefd[0]);
		//wait(NULL);
		execlp("grep","grep","-i","anand",(char *)0);	
	}
	return 0;
}
