#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

//Sleep using alarm
long n;
int i = 0;
void sigalrm(){
	//alarm(1);
	if(i < n){
		i = i + 1;
		alarm(1);
	}else{
		exit(0);
	}
}

int main(int argc,char **argv){
	n = atoi(argv[1]);
	printf("Sleeping For %ld Secs\n",n);
	signal(SIGALRM,sigalrm);
	alarm(1);
	while(1){
		pause();
	}	
	return 0;
}
