#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

char *env_init[] = { "USER=anand", "PATH=/tmp", NULL };

int main (void)
{
	pid_t pid;
	if ((pid = fork ()) < 0)
	{
		perror ("fork error");
	}
	else if (pid == 0){	/* specify pathname, specify environment */
		if (execle ("/home/anand/NetworkProgramming/lab2/echoall", "myarg1","MY ARG2", NULL,env_init) < 0)
			perror ("execle error");
	}
	if (waitpid (pid, NULL, 0) < 0)
		perror ("wait error");
	if ((pid = fork ()) < 0)
	{
		perror ("fork error");
	}
	else if (pid == 0)
	{                           /* specify filename, inherit environment */
      		if (execlp ("./echoall","only 1 arg",(char *) 0) < 0)
        	perror ("execlp error");
    	}
	exit (0);
}
