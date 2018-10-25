#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc,char *argv[]){
	
	int pipefd0[2];
	int pipefd1[2];
	int pipefd2[2];
	
	pid_t pid1,pid2;
	

	int flag = 0;
	char data[] = "anand\n";
	int i = 0;
	pipe(pipefd0);
	pipe(pipefd1);
	pipe(pipefd2);
	pid1 = fork();
	if(pid1==0){
		//child1 process
		pid2 = fork();
		if(pid2==0){
			//child2 process
			while(i < 100){
				read(pipefd1[0],data,sizeof(data));
				printf("Child 2 %d\n",getpid());
				write(pipefd2[1],data,sizeof(data));
				i++;
			}		
		}else{
			//printf("aa\n");
			while(i < 100){
				if(flag==0){
					flag = 1;
					printf("Child 1 %d\n",getpid());
					write(pipefd1[1],data,sizeof(data));
				}else{
					read(pipefd0[0],data,sizeof(data));
					printf("Child 1 %d\n",getpid());
					write(pipefd1[1],data,sizeof(data));
				}
				i++;
			}
		}
		
	}else{
		//parent process
		while(i < 100){
			read(pipefd2[0],data,sizeof(data));
			printf("Parent Process %d\n",getpid());
			write(pipefd0[1],data,sizeof(data));
			i++;
		}
	}
	
	return 0;
}
