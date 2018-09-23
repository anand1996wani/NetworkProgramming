#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

void err_sys(char* str){
	perror(str);
	exit(-1);
}

/*

2. Modify the program so that parent can accept only numbers. 
User may enter anything. 
But the program should read only the numbers

*/

#define MAXLINE 80
int main (void){
	char line[MAXLINE];
	FILE *fpin;
	if ((fpin = popen("./filter", "r")) == NULL)
		err_sys ("popen error");
	for (;;){
		fputs ("prompt> ", stdout);
		fflush (stdout);
		if (fgets (line, MAXLINE, fpin) == NULL)	/* read from pipe */
			break;
		int j = 0;
		while(line[j]!='\0'){
			if(line[j]>=48 && line[j]<=57){
				if (fputc(line[j], stdout) == EOF)
					err_sys ("fputs error to pipe");
			}
			j++;
		}
	}
	if(pclose (fpin) == -1)
		err_sys ("pclose error");
	putchar ('\n');
	exit (0);
}
