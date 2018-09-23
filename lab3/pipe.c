#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
/*

Synchronization Is achieved using sleep tring to achieve using signals 

*/
#define MSGSIZE 16

void Sigusr1Handler(){
	
}

int main (){
	int i;
	sigset_t signalMask,emptyMask;
	signal(SIGUSR1,Sigusr1Handler);
	sigaddset(&signalMask,SIGUSR1);
	sigemptyset(&emptyMask);
	sigprocmask(SIG_BLOCK,&signalMask,NULL);
	char *msg = "How are you?";
        char inbuff[MSGSIZE];
	int p[2];
	pid_t ret;
	pipe (p);
	ret = fork ();
	if (ret > 0){
		i = 0;
		while (i < 10){
			write (p[1], msg, MSGSIZE);
			//sleep (2);
			kill(ret,SIGUSR1);
			sigsuspend(&emptyMask);
			read (p[0], inbuff, MSGSIZE);
			printf ("Parent Reads Childs Data: %s\n", inbuff);
			i++;
		}
		exit(0);
    	}else{
		i = 0;
		while (i < 10){
			//sleep (1);
			sigsuspend(&emptyMask);
			read (p[0], inbuff, MSGSIZE);
			printf ("Child Reads Parents Data : %s\n", inbuff);
			write (p[1], "i am fine", strlen ("i am fine")+1);
			i++;
			kill(getppid(),SIGUSR1);
		}
		exit(0);
	}
	return 0;
}
