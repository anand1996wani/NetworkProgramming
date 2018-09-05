#include <stdio.h>
#include <signal.h>

void sigsegvHandler(){
	printf("Segmentation Fault\n");
}

int main(){
	int n;
	signal(SIGSEGV,sigsegvHandler);
	sigset_t origMask,blockMask;
	sigemptyset(&blockMask);
	sigaddset(&blockMask,SIGSEGV);
	sigprocmask(SIG_BLOCK,&blockMask,NULL);//Not Possible To block
	printf("Enter the number\n");
	scanf("%d",n);
	printf("Continue After SEGSEGV\n");
	return 0;
}
