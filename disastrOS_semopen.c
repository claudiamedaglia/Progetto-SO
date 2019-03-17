#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  int id = running->syscall_args[0];
  int count = running->syscall_args[1];
  //Controllo che non esista già un semaforo con quell'id
  Semaphore* s = SemaphoreList_byId(&semaphores_list, id);
  if(!s){ //semaforo non esiste
    s = Semaphore_alloc(id,count);
    List_insert(&semaphores_list, semaphores_list.last, (ListItem*) s);
  }
  printf("creato semaforo %d\n", id);
  //alloco il descrittore del semaforo
  SemDescriptor* d = SemDescriptor_alloc(running->last_sem_fd, s, running);
  //aumento il numero di semafori aperti
  running->last_sem_fd++;
  //alloco puntatore a descrittore
  SemDescriptorPtr* d_ptr = SemDescriptorPtr_alloc(d);
  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) d);
  d->ptr=d_ptr;
  List_insert(&s->descriptors, s->descriptors.last, (ListItem*) d_ptr);

  running->syscall_retvalue = d->fd;
  return;
  
}
