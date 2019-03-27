#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){
    int id = running->syscall_args[0];
    
    SemDescriptor* s_desc = SemDescriptorList_byFd(&running->sem_descriptors, id); //prendo descrittore del semaforo con quell'id
    if(s_desc==0){ //controllo di aver trovato il descrittore
        printf("Sempost semaforo %d fallita\n", id);
        running->syscall_retvalue = DSOS_ESEMPOST;
        return;
    }
    
    Semaphore* s = s_desc->semaphore; //prendo il semaforo
    s->count++; //incremento contatore di s
    
    if(s->count <= 0){
        List_insert(&ready_list, ready_list.last, (ListItem*) running); //metto processo running nella ready_list
    
        SemDescriptorPtr* d_ptr = (SemDescriptorPtr*) List_detach(&s->waiting_descriptors, (ListItem*) s->waiting_descriptors.first); //rimuovo il first descriptor del semaforo dalla lista dei descrittori in attesa
        List_insert(&s->descriptors, s->descriptors.last, (ListItem*) d_ptr);
        List_detach(&waiting_list, (ListItem*) d_ptr->descriptor->pcb); //rimuovo il processo dalla waiting_list tramite il suo d_ptr

        running->status = Ready;
        running = d_ptr->descriptor->pcb; //metto in esecuzione il processo rimosso dalla waiting_list
    }
    running->syscall_retvalue = 0;
    return;
}
