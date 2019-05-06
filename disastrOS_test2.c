#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"
#include "disastrOS_constants.h"

int sem1, sem2;

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n", disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("[INFO] Hello, I am the THREAD %d\n", disastrOS_getpid());

  printf("[INFO] Opening semaphores\n");

  sem1 = disastrOS_semOpen(1, 1); //lo apre correttamente
  sem2 = disastrOS_semOpen(2,-10); //mi da errore

  disastrOS_printStatus();

  disastrOS_semPost(sem2); //mi da errore
  disastrOS_semWait(sem2); //mi da errore

  printf("[INFO] pid=%d is terminating\n", disastrOS_getpid());
  printf("[INFO] Closing semaphores!\n");

  disastrOS_semClose(sem1); //ok
  disastrOS_semClose(sem1); //mi da errore
  disastrOS_semClose(sem2); // mi da errore

  disastrOS_exit(disastrOS_getpid());
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("[INFO] hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);

  int alive_children=0;  //figli vivi

  
    disastrOS_spawn(childFunction, 0);
    alive_children++;

  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n", pid, retval, alive_children);
    alive_children--;
  }

  printf("***SHUTDOWN!***\n");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0;
}
