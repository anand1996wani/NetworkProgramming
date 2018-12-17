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
int j = 1;
int flag = 0;
int iterations;
int a = 0;
int k = 0;
int childa = 0;
pid_t pid,parent;

void signalHandler(int signalNo){
	if(j <= size ){
		if(j%n==0){
			fflush(stdout);
			printf("%d (%d) \n\n",getpid(),array[j]);
			fflush(stdout);
		}else{	
			fflush(stdout);
			printf("%d (%d) \t",getpid(),array[j]);
			fflush(stdout);
		}
		j = j + n;
		kill(childa,SIGUSR1);
		//kill(getpid,SIG)
	}else{
		kill(parent,SIGINT);
	}
}


void sigintHandler(){
	if(childa!=parent)
		kill(childa,SIGINT);
	exit(0);
}

void parent1(){
	//printf("Process No %d Process Id %d Is Created \n",j,getpid());
	while(1);
	printf("Process No %d Is Exiting\n",getpid());
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
	signal(SIGUSR1,signalHandler);
	signal(SIGINT,sigintHandler);
	for(int i = 0;i <= size;i++){
		//array[i] = rand()%1000;
		array[i] = i;
		child[i] = -1;
	}
	iterations = size/n;
	a = 1;
	parent = getpid();
	if(n==1){
		while(j <= size){
			printf("%d (%d) \n\n",getpid(),array[j]);
			j++;
		}
	}else{
		for(int z = 0;z < n-1;z++){
			pid = fork();
			if(pid < 0){
				printf("Fork Error\n");
				exit(0);
			}else if(pid==0){
				j = z+2;
				if(z==n-2){
					//printf("Children Creation Successful\n");
					childa = parent;
					kill(parent,SIGUSR1);
					parent1();
				}
			}else{
				childa = pid;
				parent1();	
			}
		}
	}
	printf("\n");
	return 0;
}
