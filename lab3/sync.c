#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

void sigusr1Handler(int signo){
	
}

int main ()
{
  int i = 0, j = 0; 
  pid_t ret;
  int status;
  signal(SIGUSR1,sigusr1Handler);
  ret = fork ();
  sigset_t suspend,empty;
  sigaddset(&suspend,SIGUSR1);
  sigemptyset(&empty);
  sigprocmask(SIG_BLOCK,&suspend,NULL);
  if (ret == 0)
    {
      for (i = 0; i < 400; i++){
      	//sigprocmask(SIG_BLOCK,&suspend,NULL);
      	printf ("Child: %d\n", i);
        kill(getppid(),SIGUSR1);
      	sigsuspend(&empty);
      }
      //printf ("Child ends\n");
    }else{
      //printf ("Parent resumes.\n");
      
      for (j = 0; j < 400; j++){
      	sigsuspend(&empty);
        printf ("Parent: %d\n", j);
        kill(ret,SIGUSR1);
        //sigprocmask(SIG_BLOCK,&suspend,NULL);
     }
     wait(NULL);
     }
    return 0;
}
