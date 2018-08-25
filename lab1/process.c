#include <stdlib.h>
#include <stdio	.h>

int main(int argc, char **argv){
	printf("Process Id : \t %d \n",getpid());
	printf("Parent Process Id : \t %d \n",getppid());
	printf("User Id : \t %d \n",getuid());
	return 0;
}
