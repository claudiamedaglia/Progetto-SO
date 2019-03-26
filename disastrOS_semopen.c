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
  
  if(count < 0 ){
        printf("Allocazione semaforo con id=%d fallita\n", id);
        running->syscall_retvalue = DSOS_ESEMOPEN;
        return;
   }
  //Controllo che non esista già un semaforo con quell'id
  Semaphore* s = SemaphoreList_byId(&semaphores_list, id);

  if(s == 0){ //semaforo non esiste
    s = Semaphore_alloc(id,count);
    if(s==0){ //controllo se il semaforo è stato allocato correttam
        printf("Allocazione semaforo con id=%d fallita\n", id);
        running->syscall_retvalue = DSOS_ESEMOPEN;
        return;
    }
    List_insert(&semaphores_list, semaphores_list.last, (ListItem*) s); //lo inserisco nella lista dei semafori
  }
  printf("Semaforo con id=%d allocato correttamente\n", id);
  //alloco il descrittore del semaforo
  SemDescriptor* d = SemDescriptor_alloc(running->last_sem_fd, s, running);
  //controllo se il descrittore è stato allocato correttamente
  if(d == 0){
    running->syscall_retvalue = DSOS_ESEMOPEN;
    return;
  }
  //aumento il numero di semafori aperti
  running->last_sem_fd++;
  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*) d); //lo inserisco nella lista dei descrittori
  //alloco puntatore a descrittore
  SemDescriptorPtr* d_ptr = SemDescriptorPtr_alloc(d);
  d->ptr=d_ptr; 
  List_insert(&s->descriptors, s->descriptors.last, (ListItem*) d_ptr);

  running->syscall_retvalue = d->fd;
  return;
  
}
