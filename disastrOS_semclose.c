#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
    int id = running->syscall_args[0]; //prendo id del semaforo
    //prendo descrittore del semaforo con quell'id
    SemDescriptor* s_desc = SemDescriptorList_byFd(&running->sem_descriptors, id);
    //controllo di aver trovato il descrittore
    if(s_desc==0){
        disastrOS_debug("Chiusura semaforo fallita\n");
        running->syscall_retvalue = DSOS_ESEMCLOSE;
        return;
    }
    //lo rimuovo dalla lista dei descrittori dei semafori
    List_detach(&running->sem_descriptors, (ListItem*)s_desc);
    
    //prendo il semaforo
    Semaphore* s = s_desc->semaphore;
    //controllo di aver trovato il semaforo
    if(s==0){
          running->syscall_retvalue = DSOS_ESEMCLOSE;
          return;
    }
    //rimuovo il puntatore al descrittore dalla lista dei semafori
    SemDescriptorPtr* d_ptr = (SemDescriptorPtr*)List_detach(&s->descriptors, (ListItem*)(s_desc->ptr));

    //libero memoria
    SemDescriptor_free(s_desc);
    SemDescriptorPtr_free(d_ptr);
    
    //se non ho descrittori, rimuovo il semaforo dalla lista
    if(s->descriptors.size == 0 && s->waiting_descriptors.size==0){
        List_detach(&semaphores_list, (ListItem*)s);
        Semaphore_free(s);
    }
    
    running->syscall_retvalue = 0;
    return;
}
