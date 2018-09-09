#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void err_sys(char* str){
	perror(str);
	exit(-1);
}

static void sigQuit(int);
static void sigInt(int);
int main(void){
	sigset_t newmask, oldmask, pendmask;
	
	if (signal(SIGQUIT, sigQuit) == SIG_ERR)
		err_sys("can't catch SIGQUIT\n");
	
	if (signal(SIGINT, sigInt) == SIG_ERR)
		err_sys("can't catch SIGINT\n");
		
	sigemptyset(&newmask);
	sigaddset(&newmask,SIGQUIT);
	sigaddset(&newmask,SIGINT);
	sigfillset(&newmask);
	/* block SIGQUIT and save current signal mask */
	
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");
	
	/* critical section starting*/

	sleep(5);		
	
	/* SIGQUIT here will remain pending */

	if (sigpending(&pendmask) < 0)
		err_sys("sigpending error");

	if (sigismember(&pendmask, SIGQUIT))
		printf("\nSIGQUIT pending\n");

	if (sigismember(&pendmask, SIGINT))
		printf("\nSIGINT pending\n");
	/*critical section ending*/

	/* reset signal mask which unblocks SIGQUIT & SIGINT*/
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
	printf("SIGQUIT unblocked\n");

	sleep(5);		/* SIGQUIT SIGINT  here will terminate with core file */

	exit(0);
}

static void sigQuit(int signo){
	printf("caught SIGQUIT\n");

	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		err_sys("can't reset SIGQUIT");
	return;
	
}

static void sigInt(int signo){
	printf("caught  SIGINT\n");
	if (signal(SIGINT, SIG_DFL) == SIG_ERR)
		err_sys("can't reset SIGINT");
	return;
}
