#include <signal.h>
#include <stdio.h>

void stop_handler(int signo)
{
  printf ("Running stop_handler\n");
}

void quit_handler(int signo)
{
  printf ("Running quit_handler\n");
}

void int_handler(int signo);
main ()
{
  signal (SIGINT, int_handler);
  signal (SIGTSTP, stop_handler);
  signal (SIGQUIT, quit_handler);
  printf ("Entering infinite loop\n");
  while (1)
    {
      sleep (10);
    }
  printf (" This is unreachable\n");
}

 /* will be called asynchronously, even during a sleep */
void int_handler(int signo)
{
  printf ("Running int_handler\n");
}
