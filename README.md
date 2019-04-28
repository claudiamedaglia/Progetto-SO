# Progetto-SO
DisastrOS semaphores

**disastrOS_semOpen(int id, int count)**

Questa funzione crea un semaforo, o lo apre se già esiste, identificato da un id e da un count (numero di processi che possono entrare in "sezione critica").
Se viene chiamata una semopen con un count<0 la funzione ritorna un errore.
Se passo questo controllo, verifico se il semaforo già esiste; se non esiste lo creo tramite la Semaphore_alloc(id,count) e successivamente verifico che sia stato creato correttamente. Una volta allocato lo inserisco nella Semaphore_list (1) in ultima posizione. Alloco quindi il descrittore del semaforo appena creato e il suo puntatore, inserendoli rispettivamente nella lista dei descrittori del processo in running (2) e nella lista dei descrittori del semaforo (3). 
La funzione restituisce come valore di ritorno l'id del semaforo allocato, altrimenti un codice d'errore.

**disastrOS_semClose(int id)**

Questa funzione chiude un semaforo identificato dall'id passato come argomento, se questo esiste.
Infatti il primo controllo fatto dalla funzione è quello di andare a ricercare il descrittore del semaforo tramite il suo id nella lista dei descrittori del processo in running: se il descrittore non viene trovato viene ritornato un messaggio d'errore, altrimenti viene eliminato dalla lista dei descrittori del processo in running(2).
Rimuovo poi il puntatore del descrittore trovato dalla lista dei descrittori del semaforo (3) e libero memoria(SemDescriptor_free, SemDescriptorPtr_free).
Infine, se il processo che ha chiamato la semClose è l'ultimo processo rimasto ad aver aperto il semaforo (ovvero il numero di descrittori aperti del semaforo è uguale e 0), allora elimino il semaforo dalla Semaphore_list(1) e libero memoria (Semaphore_free).
Se tutto è andato a buon fine la funzione ha come valore di ritorno 0.

**disastrOS_semWait(int id)**

Questa funzione decrementa il count del semaforo identificato dall'id passato come argomento, se questo esiste.
Infatti il primo controllo fatto dalla funzione è quello di andare a ricercare il descrittore del semaforo tramite il suo id nella lista dei descrittori del processo in running: se il descrittore non viene trovato viene ritornato un messaggio d'errore, altrimenti tramite il descrittore mi salvo il puntatore a quest'ultimo e il semaforo, decrementandone quindi il campo count.


