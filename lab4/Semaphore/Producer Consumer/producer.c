#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY (14923)
#define KEY1 (2003)


union semun
{
  int val;			
  struct semid_ds *buf;		/* Buffer for IPC_STAT, IPC_SET */
  unsigned short *array;	
  struct seminfo *__buf;	
};


int main (){
	int id,binarySem;	
	//struct sembuf operations[2];
	struct sembuf operations;
	struct sembuf binary;

	int retval = 0;
	union semun getval;
	unsigned short val[1];
	id = semget (KEY, 1, IPC_CREAT|0666);
	binarySem = semget(KEY1,1,IPC_CREAT|0666);
	if (id < 0){
		fprintf (stderr, "Producer cannot find semaphore, exiting.\n");
		exit (0);
	}
	
	semctl(binarySem,0,SETVAL,1);
	semctl(id,0,SETVAL,10);
	/*
   Set up the sembuf structure. 
	operations[0].sem_num = 0;
	operations[0].sem_op = 0;
	operations[0].sem_flg = 0;



   Set up the sembuf structure. 
	operations[1].sem_num = 0;
   add 10 to sem value 
	operations[1].sem_op = 10;
	operations[1].sem_flg = 0;
*/

	binary.sem_num = 0;
	binary.sem_op = 0;
	binary.sem_flg = 0;
	
	operations.sem_num = 0;
	operations.sem_op = 1;
	operations.sem_flg = 0;
		
	
	for (;;){
		if (retval == 0){
			printf ("Producer: Adding 1 objects\n");
			getval.array = val;
			semctl (id, 0, GETALL, getval);
			printf ("Sem Val: %d\n", getval.array[0]);
		}else{
			printf ("semop: opeartion did not succeed.\n");
			perror ("REASON");
		}
		binary.sem_op = 0;
		retval = semop(binarySem,&binary,1);
		
		retval = semop(id, &operations, 1);
		
		binary.sem_op = 1;
		retval = semop(binarySem,&binary,1);
		
      		sleep (1);
    	}
    	return 0;
}

