#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
 
#define key (2000)

union semun {
	int val;
	struct semid_ds * buf;
	unsigned short * array;
	#if defined(__linux__)
		struct seminfo * __buf;
	#endif
};

struct semid_ds ds;
union semun arg;


int main(int argc,char *argv[]){
	int id = 0;
	
	id = semget(key,1,IPC_CREAT | 0666);
	
	if(id < 0){
		printf("Semaphore Is Not Created \n");
	}else{
		printf("Semaphore Successfully Created With Id %d \n",id);
	}
		
	pid_t pid;
	arg.buf = &ds;
	
	pid = fork();
	if(pid < 0){
		printf("Fork Error\n");
	}else if(pid==0){
		//child process
		sleep(5);
		semctl(id,0,SETVAL,arg);
		/*if(ds.sem_otime!=0){
			printf("Other process has Performed Semop without initialization of Semaphore\n");
		}*/
	}else{
		sleep(5);
		
		struct sembuf sops;
		sops.sem_num = 0;
		sops.sem_op = -1;
		sops.sem_flg = SEM_UNDO;
		semop(id,&sops,1);
		if(ds.sem_otime!=0){
			printf("Other process has Performed Semop without initialization of Semaphore\n");
		}
		//wait(NULL);
	}
	
	return 0;
}
