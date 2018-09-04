#include <stdio.h>
#include <signal.h>

void sigsegvHandler(){
	printf("Segmentation Fault\n");
}

int main(){
	int n;
	signal(SIGSEGV,sigsegvHandler);
	printf("Enter the number\n");
	scanf("%d",n);
	printf("Continue After SEGSEGV\n");
	return 0;
}
