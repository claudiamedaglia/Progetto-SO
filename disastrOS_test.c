#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"
#include "disastrOS_constants.h"

#define BUFFER_LENGTH 50
#define ITERATION_COUNT 10

int filled_sem, empty_sem, read_sem, write_sem;
int buf[BUFFER_LENGTH];
int buffer_length = BUFFER_LENGTH;
int write_index = 0;
int read_index = 0;

unsigned long int shared_variable;

int produttore(){
    disastrOS_semWait(empty_sem);
    disastrOS_semWait(write_sem);
    
    int ret = shared_variable;
    buf[write_index] = shared_variable;
    write_index = (write_index+1) % buffer_length;
    shared_variable++;

    disastrOS_semPost(write_sem);
    disastrOS_semPost(filled_sem);
    
    return ret;
}

int consumatore(){
    int x;
    
    disastrOS_semWait(filled_sem);
    disastrOS_semWait(read_sem);
    
    x = buf[read_index];
    read_index = (read_index+1) % buffer_length;
    
    disastrOS_semPost(read_sem);
    disastrOS_semPost(empty_sem);
    
    return x;
}

// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n", disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("[INFO] I am the THREAD %d\n", disastrOS_getpid());

  printf("[INFO] Opening semaphores by THREAD %d\n", disastrOS_getpid());

  filled_sem = disastrOS_semOpen(1, 0);  //semaforo che mi dice se buffer pieno
  empty_sem = disastrOS_semOpen(2, buffer_length); //semaforo che mi dice se buffer vuoto
  read_sem = disastrOS_semOpen(3, 1);  //semaforo di lettura
  write_sem = disastrOS_semOpen(4,1); //semaforo di scrittura

  //disastrOS_printStatus();
  for(int i=0; i < ITERATION_COUNT; i++){
    if (disastrOS_getpid() % 2 == 0){
     int sv = produttore();
     printf("[ACTION] THREAD %d: ho inserito nel buffer il valore %d\n", disastrOS_getpid(), sv);
     }
    else {
     int ret = consumatore();
     printf("[ACTION] THREAD %d: ho letto dal buffer il valore %d\n", disastrOS_getpid(), ret);
    }
  }

  printf("[INFO] pid=%d is terminating\n", disastrOS_getpid());

  printf("[INFO] Closing semaphores by THREAD %d\n", disastrOS_getpid());
  disastrOS_semClose(filled_sem);
  disastrOS_semClose(empty_sem);
  disastrOS_semClose(read_sem);
  disastrOS_semClose(write_sem);
  disastrOS_exit(disastrOS_getpid());
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("[INFO] START\n");
  //disastrOS_spawn(sleeperFunction, 0);

  printf("-----STATO INIZIALE DEL BUFFER----\n");
  for(int i=0; i < buffer_length; i++){
    printf("%d ", buf[i]);
  }
  printf("\n");

  shared_variable=1; //fisso valore variabile globale

  printf("[INFO] I'm creating 10 threads...\n");
  int alive_children=0;  //figli vivi

  for (int i=0; i<10; ++i) {  //creo 10 threads
    disastrOS_spawn(childFunction, 0);
    printf("[INFO] %dth thread created\n", i+1);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n", pid, retval, alive_children);
    alive_children--;
  }
  
  printf("----- STATO FINALE DEL BUFFER ----\n");
  for(int i=0; i < buffer_length; i++){
    printf("%d ", buf[i]);
  }
  printf("\n");

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
