#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define key (20091011)
//semaphore has kernel persistance
//Number of semaphores created by your account is ipcs -s
//ipcs -s -i <semid> check the value of semaphores

int main (int argc,char *argv[]){
	int id;
	id = semget (key, 1, IPC_CREAT | 0666);
	if (id < 0)
		printf ("Semaphore is not created\n");
	else
		printf("Semaphore successfully created with id %d\n", id);
		
	//Modification Of Default values of Semaphores
	semctl(id,0,SETVAL,32767);
	// semctl(semid,semnum,opeartion,value);
	//Max value of semaphore is 32767
	
	//For binary semaphore we initialize semaphore value to 1 and for counting semaphore we initilize it to count 
	semctl(id,0,IPC_RMID,0);//Removing semaphore from system
	/*
	
		 Immediately remove the semaphore set, awakening all processes
	         blocked in semop(2) calls on the set (with  an  error  return
                 and  errno set to EIDRM).  The effective user ID of the call‐
                 ing process must match the creator or owner of the  semaphore
                 set,  or  the caller must be privileged.  The argument semnum
                 is ignored.
	
	*/
	return 0;
}
