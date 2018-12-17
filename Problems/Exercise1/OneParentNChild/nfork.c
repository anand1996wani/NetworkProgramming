#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define size 1000
int array[1001];
int n = 0;
int child[1001];
int j = 0;
int flag = 0;
int iterations;
int a = 0;
int k = 0;

void signalHandler(int signalNo){
	if(j%n==0){
		fflush(stdout);
		printf("Child %d (%d) \n\n",getpid(),array[j]);
		fflush(stdout);
	}else{	
		fflush(stdout);
		printf("Child %d (%d) \t",getpid(),array[j]);
		fflush(stdout);
	}
	j = j + n;
	kill(getppid(),SIGUSR2);
	//exit(0);
}


void handler(int signalNo){
	if(a <= size){
		//printf("In Parent Handler %d \n",a);
		kill(child[(a-1)%n],SIGUSR1);
		a = a + 1;
		//exit(0);
	}else{
		printf("\n");
	}
}


void parentHandler(int signalNo){
	for(int z = 0;z < n;z++){
		printf("Killing Process %d \n",child[z]);
		kill(child[z],SIGINT);
		wait(NULL);
		sleep(1);
	}
	printf("Parent Is Exiting\n");
	exit(0);
}
void child1(){
	//printf("Child No %d Process Id %d Is Created\n",j,getpid());
	while(1);
	printf("Child No %d Is Exiting\n",getpid());
	exit(0);
}


int main(int argc,char **argv){
	//printf("Enter Number Of Processes\n");
	//scanf("%d",&n);
	if(argc < 2){
		printf("Please Enter Number Of Processes Via Command Line Arguments\n");
		exit(0);
	}
	n = atoi(argv[1]);
	signal(SIGUSR2,handler);
	signal(SIGUSR1,signalHandler);
	for(int i = 0;i <= size;i++){
		//array[i] = rand()%1000;
		array[i] = i;
		child[i] = -1;
	}
	pid_t pid;
	iterations = size/n;
	a = 1;
	for(int z = 0;z < n;z++){
		pid = fork();
		if(pid < 0){
			printf("Fork Error\n");
			exit(0);
		}else if(pid==0){
			j = z+1;
			if(z==n-1)
				kill(getppid(),SIGUSR2);
			child1();
		}else{
			child[k++] = pid;
		}
		//sleep(1);
	}
	signal(SIGINT,parentHandler);
	//kill(child[0],SIGUSR1);
	while(1);
	printf("\n");
	return 0;
}
