#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
int c = 0;
void forkme(int n){
	if(n > 0){
		fork();
		c++;
		forkme(n-1);
	}
}

int main(){
	
	int val = 5;
	//0 is false and non zero is true
	int a = 0;
	int status=99;
	if(fork()){
		a = wait(&status);
	}
	val++;
	printf("%d\n",status);
	printf("%d\n",val);
	printf("%d\n",a);
	/*
	int pid1,pid2;
	printf("FIRST\n");
	pid1=fork();
	if(pid1==0) {
		printf("SECOND \n");
		pid2=fork();
		printf("SECOND \n");
	}else{
		printf("THIRD\n");
	}
	output
	FIRST
	THIRD
	SECOND 
	SECOND 
	SECOND
	*/	
	
		//forkme(4);
		//printf("%d\n",c);	
	return 0;
}
