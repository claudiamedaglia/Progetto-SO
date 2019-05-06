#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semWait(){
  int id = running->syscall_args[0]; //prendo l'id del semaforo

  SemDescriptor* s_desc = SemDescriptorList_byFd(&running->sem_descriptors, id); //mi prendo il descrittore del semaforo tramite l'id
  if(s_desc==0){ //controllo di aver trovato il descrittore
        printf("    [SEMAFORO] Semwait semaforo con id=%d fallita\n", id);
        running->syscall_retvalue = DSOS_ESEMWAIT;
        return;
    }
    
    Semaphore* s = s_desc->semaphore; //mi salvo il semaforo
    SemDescriptorPtr* d_ptr = s_desc->ptr; //mi salvo il puntatore del descrittore

    s->count--; //decremento il contatore del semaforo

    
    if (s->count < 0){
        List_detach(&s->descriptors, (ListItem*)d_ptr); //rimuovo il descrittore del processo dalla lista dei descrittori
        List_insert(&s->waiting_descriptors, s->waiting_descriptors.last, (ListItem*) s_desc->ptr); //lo inserisco nella lista di waiting
        running->status = Waiting; 

        List_insert(&waiting_list, waiting_list.last, (ListItem*) running); //inserisco il processo in esecuzione nella waiting_list
       
        PCB* pcb = (PCB*) List_detach(&ready_list, (ListItem*)ready_list.first); //prendo il primo elemento dalla ready list
        running = (PCB*)pcb; //mando in esecuzione il primo elemento della lista di ready appena preso
    }

    running->syscall_retvalue = 0;
    

    return;
}
