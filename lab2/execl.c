#include <stdio.h>
#include <stdlib.h>

//Why is it stated that exec calls don’t return anything upon success?

int main (int argc,char **argv)
{
	//execl("/bin/ls","-l",(char*) 0);	
	//execlp ("ls","", NULL);
	execv("/bin/ls",argv);
	printf ("hello");
}
