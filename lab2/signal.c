#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void stop_handler(int signo)
{
  printf ("Running stop_handler\n");
}

void quit_handler(int signo)
{
  printf ("Running quit_handler\n");
}

void int_handler(int signo);
int main ()
{
  signal (SIGINT, int_handler);
  signal (SIGTSTP, stop_handler);
  signal (SIGQUIT, quit_handler);
  sigset_t originalMask,blockMask,waitingMask;
  sigaddset(&blockMask,SIGINT);
  sigaddset(&blockMask,SIGQUIT);
  sigaddset(&blockMask,SIGTSTP);
  sigprocmask(SIG_BLOCK,&blockMask,&originalMask);
  printf ("Entering infinite loop\n");
  int i = 0;
  while (i==0)
    {
      sleep (30);
      i++;
    }
    sigpending(&waitingMask);
    if(sigismember(&waitingMask,SIGINT)){
    	printf("Pending Signal Is Ctrl-C\n");
    }
    if(sigismember(&waitingMask,SIGTSTP)){
    	printf("Pending Signal Is Ctrl-Z\n");
    }
    if(sigismember(&waitingMask,SIGQUIT)){
    	printf("Pending Signal Is SigQuit\n");
    }
  printf (" This is unreachable\n");
  return 0;
}

 /* will be called asynchronously, even during a sleep */
void int_handler(int signo)
{
  printf ("Running int_handler\n");
}
