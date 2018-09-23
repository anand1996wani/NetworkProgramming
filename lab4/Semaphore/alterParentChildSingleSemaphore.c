#include <stdio.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define key (2000)


union semun {
	int val;
	struct semid_ds * buf;
	unsigned short * array;
	#if defined(__linux__)
		struct seminfo * __buf;
	#endif
};


int main(int argc,char *argv[]){
	int semid,semid1;
	semid = semget(key,2,IPC_CREAT | 0666);
	if(semid==-1){
		printf("Sem Error");
		exit(1);
	}
	union semun arg;
	int status;
	arg.val = 0;	//set initial value of sem[0]
	status = semctl(semid,0,SETVAL,arg);
	
	arg.val = 1;
	status = semctl(semid,1,SETVAL,arg);	//set initial value of sem[1]
	pid_t pid;
	struct sembuf sops;
	//sops.sem_num = 0;
	sops.sem_flg = SEM_UNDO;
	pid = fork();
	if(pid < 0){
		printf("Fork Error \n");
		exit(1);
	}else if(pid==0){
		//child process
		for(int i = 1;i < 100;i=i+2){
			sops.sem_num = 0;
			sops.sem_op = -1;	//dec sem[0]
			semop(semid,&sops,1);	
			printf("Child %d : %d\n",getpid(),i);
			sops.sem_num = 1;
			sops.sem_op = 1;	//inc sem[1]
			semop(semid,&sops,1);	
		}
	}else{
		//parent process
		for(int i = 0;i < 100;i=i+2){
			sops.sem_num = 1;	
			sops.sem_op = -1;	//dec sem[1]
			semop(semid,&sops,1);
			printf("Parent %d : %d\n",getpid(),i);
			sops.sem_num = 0;
			sops.sem_op = 1;	//inc sem[0]
			semop(semid,&sops,1);		
		}
		
		
		wait(NULL);
	}
	return 0;
}
