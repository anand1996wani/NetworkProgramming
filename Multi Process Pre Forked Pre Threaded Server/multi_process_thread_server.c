#include<stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include<signal.h>
#include<errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>
#include<errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#define MAX_CLIENTS 100
#define checkError(eno,msg)               \
  ({                                      \
          if((eno)<0)                       \
          {                               \
                  perror(msg);            \
                  exit(EXIT_FAILURE);     \
         }                                \
 })
int status = 0;
static int nchildren;		// number of children
static short PORT;
static int nthreads;		// no. of threads per child process
static int nconns;		// no. of conns per process
pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;	//for adding number of connections
pthread_mutex_t sigmain = PTHREAD_MUTEX_INITIALIZER;	//mutex for signaling the main thread to terminate the process
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;	//main thread will wait here

pid_t parentpid;			// save pid of exited children processes.
pid_t pid;
//shared memory 
struct client
{
  long id;
  char name[10];
};
struct client *cp;
struct client *c;

int nc1 = -1;
int *numclients = &nc1;
int shmid = 0;
int shmid1 = 0;
//declare the message queue.
int queueId = 0;
struct payload
{
  char srcid[10];

  char msg[484];

};
//message queue structure
struct msgq
{
  long mtype;
  struct payload pload;
};
//semaphore things included here.
int semid;
struct sembuf waitop[1], signalop[1];
union
{
  int val;
  struct semid_ds *buf;
  unsigned short *array;
} setval;
int ppconns = 0;
int activeconns = 0;

//deallocates all the kernel persistent resources
void
dellocate ()
{
  checkError (shmdt (cp), "Deattaching shared memory failed.");
  checkError (shmctl (shmid, IPC_RMID, NULL),
	      "Removing shared memory failed.");
  checkError (shmdt (numclients), "Deattaching shared memory failed.");
  checkError (shmctl (shmid1, IPC_RMID, NULL),
	      "Removing shared memory failed.");
  checkError (semctl (semid, 0, IPC_RMID), "Removing Semaphore failed.");
  checkError (msgctl (queueId, IPC_RMID, NULL),
	      "Removing Message queue failed.");
}


//signal handler for SIGINT for SIGQUIT.

void
signalhandler (int signo)
{

	if(parentpid!=getpid())
	exit(EXIT_SUCCESS);
	else
	while ((pid = waitpid (0, &status, WNOHANG)) > 0)
    printf ("Process with pid %d has exited\n", pid);
  dellocate ();
  
  exit (EXIT_SUCCESS);
}

//get listen fd of the server given a port
int
getListenfd (short PORT, struct sockaddr_in *address)
{
  int server_fd = socket (AF_INET, SOCK_STREAM, 0);
  checkError (server_fd, "TCPSOCKETfailed");
  int rc = 0;
  int opt = 1;
  //make the socket port reusable and other boiler plate for creating a socket
  checkError (setsockopt
	      (server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
	       sizeof (opt)), "SetSockTCPFailed");
  address->sin_family = AF_INET;
  address->sin_addr.s_addr = INADDR_ANY;
  address->sin_port = htons (PORT);
  rc = bind (server_fd, (struct sockaddr *) address, sizeof (*address));
  checkError (rc, "BindSockTCPFailed");
  rc = listen (server_fd, 3);
  checkError (rc, "ListenTCP");
  return server_fd;
}

//Get messages from the message queue
void
receive_msg (int connfd, long mtype)
{
  struct msgq r;
  int ret = 0;
  int snd = 0;
  r.mtype = mtype;

  while (ret >= 0)
    {
      ret = msgrcv (queueId, (void *) &r, sizeof (r), r.mtype, IPC_NOWAIT);
      if (ret >= 0)
	{


	  char msg[500];
	  memset (msg, 0, 500);
	  strcpy (msg, r.pload.srcid);
	  strcat (msg, " SAID :");
	  strcat (msg, r.pload.msg);

	  snd = send (connfd, msg, sizeof (msg), 0);
	  checkError (snd, "sendErrorinreceivemsg");
	}
    }
}

//get corresponding ids/mtypes for clients that were stored in the shared memory c
int
fetchmtype (char *destid, int nc)
{
  for (int i = 0; i <= nc; i++)
    {
      printf ("names AT I index in shared memory is %d %s\n", i, c[i].name);
      if (strcmp (destid, c[i].name) == 0)
	return c[i].id;

    }
  return -1;
}

//returns the first empty index in the shared memory.
int
getemptyindex (int nc, char *uid)
{
  for (int i = 0; i <= nc; i++)
    {
      if (strcmp (uid, c[i].name) == 0)
	return i;
      else
	{
	  if (c[i].id == 0)
	    return i;
	}
    }
}

//main function for each client that has all the chat functionality.
void
handle_client (int connfd, struct sockaddr *clientaddr)
{
  char buffer[520];
  char userid[10];

  char *token;
  char cmd[5];
  char destid[10];
  char msg[484];
  memset (msg, 0, 484);
  memset (cmd, 0, 5);
  memset (destid, 0, 10);
  memset (userid, 0, 10);
  struct msgq m;
  int index = 0;
  int clientid = 0;


  unsigned long mtype = 0;
  int loggedin = 0;

  while (1)
    {
      char *rest = buffer;
	  if(mtype>0)
      	receive_msg (connfd, mtype);
      int valread = recv (connfd, buffer, sizeof (buffer), MSG_DONTWAIT);	
      if (valread == -1)
	{
	//Read has returned -1 so read again.
	  continue;
	}

      if (!(strchr (buffer, '\n')))
	{
	//just searches for a new line in the provided string.
	  continue;
	}
      checkError (valread, "readblocked");
      buffer[valread] = '\0';

      token = strsep (&rest, " ");
	
      strncpy (cmd, token, 4);

      if ((strcmp (cmd, "JOIN") == 0) && loggedin == 0)
	{
	  	if(rest==NULL)
		{
			 send (connfd, "Please Enter a user name!\n", strlen ("Please Enter a user name!\n"), 0);
			continue;
		}
	  loggedin = 1;
	  char uid[10];
	  token = strsep (&rest, "\r\n ");
	  strncpy (userid, token, 10);
	  printf ("JOINED ID:[%s] %ld\n", userid, strlen (userid));
	  strncpy (uid, userid, 10);

	  //set the client index first.
	  checkError (semop (semid, waitop, 1), "SEMOPERROR");
	  if ((*numclients) <= MAX_CLIENTS)
	    {
	      *numclients = *numclients + 1;
	      index = getemptyindex ((*numclients), uid);
	      checkError (semop (semid, signalop, 1), "SEMOPERROR");
	      clientid = index + 1;
	      c[index].id = clientid;

	    }
	  else
	    {
	      exit (EXIT_FAILURE);
	    }
	  mtype = clientid;
	  char dest[] = "WELCOME ";
	  strcat (dest, uid);
	  send (connfd, dest, strlen (dest), 0);
	  send (connfd, "\n", 1, 0);
	  strncpy (userid, uid, 10);
	  checkError (semop (semid, waitop, 1), "SEMOPERROR");
	  strcpy (c[index].name, userid);
	  checkError (semop (semid, signalop, 1), "SEMOPERROR");
	  receive_msg (connfd, mtype);
	  continue;
	}
      else if ((strcmp (cmd, "CHAT") == 0) && loggedin == 1)
	{
	     		if(rest==NULL)
		{
			 send (connfd, "Please Enter the destination\n", strlen ("Please Enter the destination\n"), 0);
			continue;
		}
	  token = strsep (&rest, " ");
				if(rest==NULL)
		{
			 send (connfd, "Please Enter the message\n", strlen ("Please Enter the message\n"), 0);
			continue;
		}
	  strncpy (destid, token, 10);
	  checkError (semop (semid, waitop, 1), "SEMOPERROR");
	  m.mtype = fetchmtype (destid, *numclients);
	  checkError (semop (semid, signalop, 1), "SEMOPERROR");
	  if (m.mtype == -1)
	    {
	      char *i = "NO SUCH USER EXISTS\n";
	      send (connfd, i, strlen (i), 0);
	      continue;
	    }
	 // printf ("destid mtype is %ld\n", m.mtype);
	  strncpy (m.pload.srcid, userid, 10);
	  strncpy (msg, rest, 484);
	  strncpy (m.pload.msg, msg, 484);
	  int ret = msgsnd (queueId, &m, sizeof (m.pload), 0);
	  checkError (ret, "ERRORinmsgQ");
	  receive_msg (connfd, mtype);
	  continue;
	}
      else if ((strcmp (cmd, "LEAV") == 0) && loggedin == 1)
	{


	  receive_msg (connfd, mtype);
	  printf ("LEFT ID:%s\n", userid);
	   send (connfd, "Goodbye See you soon!\n", strlen ("Goodbye See you soon!\n"), 0);
	  int rc = pthread_mutex_lock (&mlock);
	  checkError (rc, "MUTEXERROR\n");
	  activeconns--;
	  rc = pthread_mutex_unlock (&mlock);
	  checkError (rc, "MUTEXERROR\n");
	  rc=close(connfd);
	  checkError (rc, "ConnfdERROR\n");
	  break;

	}
      else if (strcmp (cmd, "HELP") == 0)
	{
	  char *help[5] =
	    { "PLEASE USE THE FOLLOWING COMMANDS TO CONNECT:\n",
"JOIN <USER_NAME>:LOGIN \n", "CHAT <DEST NAME> <MESSAGE>:SEND MESSAGE\n",
"LEAV : LEAVE THE ROOM\n", "HELP:FOR MORE INFO\n" };
	  for (int i = 0; i < 5; i++)
	    send (connfd, help[i], strlen (help[i]), 0);
		continue;

	}
      else
	{
	  char *i = "INVALID USER/ENTER HELP FOR MORE INFORMATION\n";
	  send (connfd, i, strlen (i), 0);

	}

    }

}

//main function for each thread. this counts the no. of per process threads and accordingly signals the main thread
static void *
thread_main (void *arg)
{
  int rc = 0;
  int listenfd = *((int *) arg);
  free (arg);
  rc = pthread_detach (pthread_self ());
  checkError (rc, "DETACHERROR\n");
  int connfd = 0;
  socklen_t clientaddr_len;
  struct sockaddr *clientaddr;
  clientaddr = malloc (clientaddr_len);

  for (;;)
    {

	//lock ppconns before accessing
      rc = pthread_mutex_lock (&mlock);
      checkError (rc, "MUTEXERROR\n");
      if (ppconns < nconns)
	{
	  printf ("thread id is %ld \n", pthread_self ());

	  connfd = accept (listenfd, clientaddr, &clientaddr_len);
	  checkError (connfd, "ACCEPTFAILED\n");
	  //checkError(close(listenfd),"closeError");
	  printf ("Process with id %d and thread %ld is accepting client\n",
		  getpid (), pthread_self ());
	  printf ("nconns before value is %d\n", nconns);
	  printf ("ppconns before value is %d\n", ppconns);
	  activeconns++;	// so that activeconns doesnt become zero first before ppconns becomes equal to nconns.
	  ppconns++;
	  char msg[] = "TYPE HELP FOR MORE INFORMATION\n";
	  send (connfd, msg, strlen (msg), 0);
	  rc = pthread_mutex_unlock (&mlock);
	  checkError (rc, "MUTEXERROR\n");
	  handle_client (connfd, clientaddr);

	}

      else
	{
	  rc = pthread_mutex_unlock (&mlock);
	  checkError (rc, "MUTEXERROR\n");
	  rc = pthread_mutex_lock (&mlock);
	  checkError (rc, "MUTEXERROR\n");
	  if (activeconns <= 0)
	    {
	      printf ("Active conns is %d\n", activeconns);


	      rc = pthread_cond_signal (&cond);
	      checkError (rc, "CONDsignalERROR\n");
	      rc = pthread_mutex_unlock (&mlock);
	      checkError (rc, "MUTEXERROR\n");

	    }
	  else
	    {
		//release the lock incase control didnt enter if.
	      rc = pthread_mutex_unlock (&mlock);
	      checkError (rc, "MUTEXERROR\n");
	    }

	}

    }
  return (NULL);

}

//function for creation of threads.
void
thread_make (int i, int listenfd)
{
  pthread_t thread_id;
  int *lfd = malloc (sizeof (int));
  *lfd = listenfd;

  pthread_create (&thread_id, NULL, &thread_main, lfd);
}

void
child_main (int i, int listenfd, int *pfd)
{

  printf ("child %d with pid %d starting\n", i, getpid ());
  printf ("nthreads=%d\n", nthreads);
  for (int i = 0; i < nthreads; i++)
    {
      printf ("Thread pool created\n");
      thread_make (i, listenfd);	//create nthreads here
    }
  int rc = pthread_mutex_lock (&mlock);
  checkError (rc, "MUTEXERROR\n");

  if(ppconns < nconns){
  	rc = pthread_cond_wait (&cond, &mlock);
    checkError (rc, "CONDVARERROR\n");
  }
  
  /*while (ppconns < nconns)
    {
      rc = pthread_cond_wait (&cond, &mlock);
      checkError (rc, "CONDVARERROR\n");
    }*/
  rc = pthread_mutex_unlock (&mlock);
  checkError (rc, "MUTEXERROR\n");

  //Wrting in the pipe to indicate to the parent process.
  write (pfd[2 * i + 1], (char *) &i, 1);
  close (pfd[2 * i + 1]);
  //printf ("Done writing in end %d\n", 2 * i + 1);
  exit (0);
}
//function for creation of child processes.
pid_t
child_make (int i, int listenfd, int *pfd)
{
  pid_t pid;
  pid = fork ();
  int rc = 0;
  checkError (pid, "ForkError");
  if (pid == 0)
    {
      rc = close (pfd[2 * i]);	//close read end of the pipe
      checkError (rc, "PIPWRITEFAILURE\n");
      child_main (i, listenfd, pfd);	//child never returns

    }

  return pid;			// parent returns

}

int
main (int argc, char **argv)
{

  if (argc != 5)
    {
      printf
	("Not enough arguments! Order of arguments is <nchildren> <PORT> <nthreads> <nconns>\n");
      exit (EXIT_FAILURE);
    }

	signal(SIGINT,signalhandler);
	signal(SIGQUIT,signalhandler);
  //create the shared memory.
  int size = sizeof (struct client) * MAX_CLIENTS;
  shmid = shmget (IPC_PRIVATE, size, 0666 | IPC_CREAT);
  checkError (shmid, "SHAREDMEMERROR");
  shmid1 = shmget (IPC_PRIVATE, sizeof (int), 0666 | IPC_CREAT);
  checkError (shmid1, "SHAREDMEMERROR");
  checkError (semid =
	      semget (IPC_PRIVATE, 1, IPC_CREAT | 0600), "SEMPHOREERROR");
  waitop[0].sem_num = 0;
  waitop[0].sem_op = -1;
  waitop[0].sem_flg = 0;
  signalop[0].sem_num = 0;
  signalop[0].sem_op = 1;
  signalop[0].sem_flg = 0;
  setval.val = 1;
  semctl (semid, 0, SETVAL, setval);
  cp = (struct client *) shmat (shmid, (void *) 0, 0);
  c = cp;
  numclients = (int *) shmat (shmid1, (void *) 0, 0);
  *numclients = -1;
  if (cp == (void *) -1)
    {
      perror ("shmattacherr");
      exit (1);
    }
  parentpid=getpid();
  bzero (cp, size);
  nchildren = atoi (argv[1]);
  int *pfd = calloc (2 * nchildren, sizeof (int));	//for the pipefds
  for (int i = 0; i < nchildren; i++)
    {
      int rc = pipe (&pfd[2 * i]);
      checkError (rc, "PIPEERROR\n");
    }
  pid_t *pids = calloc (nchildren, sizeof (pid_t));
  struct sockaddr_in address;
  PORT = atoi (argv[2]);
  nthreads = atoi (argv[3]);
  nconns = atoi (argv[4]);
  memset (&address, 0, sizeof (address));
  int listenfd = getListenfd (PORT, &address);
  fd_set allset, tmpset;
  FD_ZERO (&allset);

  int i = 0, max = 0;
  queueId = msgget (IPC_PRIVATE, IPC_CREAT | 0666);
  checkError (queueId, "ERRORincreatingmsgQ");
  for (i = 0; i < nchildren; i++)
    {
      pids[i] = child_make (i, listenfd, pfd);
      if (max < pfd[2 * i])
	max = pfd[2 * i];
      FD_SET (pfd[2 * i], &allset);
    }
//select is used to check for signals from the child processes and create child processes accordingly.
  while (1)
    {
      allset = tmpset;
      select (max + 1, &allset, NULL, NULL, NULL);
    //  printf ("Become readable\n");
      //reclaim the child that exited after writing in the pipe.
      while ((pid = waitpid (0, &status, WNOHANG)) > 0)
	printf ("Process with pid %d has exited\n", pid);
      int buff = 0;
     // printf ("pfd value is %d and FD_ISSET=%d\n", pfd[0],
	   //   FD_ISSET (pfd[0], &allset));
      for (i = 0; i < nchildren; i++)
	{
	  if (FD_ISSET (pfd[2 * i], &allset))
	    {
	   //   printf ("Process %d has written before in the pipe\n", i);
	      read (pfd[2 * i], &buff, 1);
	   //   printf ("Process %d has written in the pipe", buff);
	      pids[i] = child_make (i, listenfd, pfd);
	 //     printf ("process with %d id created\n", pids[i]);
	    }

	}

    }
}
