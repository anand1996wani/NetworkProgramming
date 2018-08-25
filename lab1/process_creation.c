#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int glob = 6;
int main(int argc,char **argv){
	int local = 7;
	pid_t pid;
	//pid = fork();
	pid = vfork();// child uses same page tables as of parent
	if(pid < 0){
		printf("Fork Error\n");
	}else if(pid==0){
		printf("Child\n");
		printf("Process Id %d \n",getpid());
		printf("Parent Id Is %d \n",getppid());
		//local++;
		//glob++;
		printf("Local Variable Is %d\n",local);
		printf("Global variable Is %d\n",glob);
		exit(0);
	}else{
		
		printf("Parent\n");
		printf("Process id %d \n",getpid());
		glob++;
		printf("Parent Id Is %d \n",getppid());
		printf("Local Variable Is %d\n",local);
		printf("Global variable Is %d\n",glob);
		exit(0);
	}
	return 0;
}
