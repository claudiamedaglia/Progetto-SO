#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  int fd = running->syscall_args[0];
  //int c = running->syscall_args[1];
  //alloco il semaforo
  Semaphore* s = Semaphore_alloc(fd,1);
  List_insert(&semaphores_list, semaphores_list.last, (ListItem*) s);
  //alloco il descriptor del semaforo
  SemDescriptor* d = SemDescriptor_alloc(running->last_sem_fd, s, running);
  //aumento il numero di semafori aperti
  running->last_sem_fd++;
  //alloco puntatore a descrittore
  SemDescriptorPtr* d_ptr = SemDescriptorPtr_alloc(d);
  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) d);
  d->ptr=d_ptr;
  List_insert(&s->descriptors, s->descriptors.last, (ListItem*) d_ptr);
  running->syscall_retvalue = d->fd;
  printf("creato semaforo %d\n", fd);
  return;
  
}
