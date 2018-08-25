#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc,char **argv){
	extern char **environ;
	char **ptr;
	//Printing Environment Variables
	printf("Environment Variables\n");
	/*
	putenv() to set an environment variable requires that the string passed into putenv must be static, or malloced because it just 	sets a pointer to it in the environment.
	
	while setenv() allocates separate memory for the environment variable
	*/
	
	char anand[] = "rahulwani";
	int status = setenv(anand,anand,1);
	//status = unsetenv(anand);	0 on success and -1 on error
	//int status = putenv(anand); // 0 on success and nn zero on error
	printf("status = %d\n",status);// 0 on success and -1 on error
	if(status==0){
		printf("Value Set Successfully\n");
	}else{
		printf("Error In  Setting Enviro Variable\n");
	}
	strcpy(anand,"ab");
	for(ptr = environ;*ptr!=0;ptr++){
		printf("%s\n",*ptr);
	}
	
	//Printing Command Line Arguments
	printf("Command Line arguments\n");
	for(int i = 0;i < argc;i++){
		printf("%s\n",argv[i]);
	}
	return 0;
}
