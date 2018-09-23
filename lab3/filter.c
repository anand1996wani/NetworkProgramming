#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void err_sys (char *str){
	perror (str);
	exit (-1);
}

int main(){
	int c;
	while ((c = getchar ()) != EOF){
		if (islower (c))
			c = toupper (c);
		if (putchar (c) == EOF)
			err_sys("output error");
		if (c == '\n')
			fflush (stdout);
    	}
	exit (0);
}
