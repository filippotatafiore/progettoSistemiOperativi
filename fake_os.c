#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "fake_os.h"



void FakeOS_init(FakeOS* os) {  //inizializza i campi di del FakeOS* os
  List_init(&os->running);
  List_init(&os->ready);
  List_init(&os->waiting);
  List_init(&os->processes);
  os->timer=0;
  os->schedule_fn=0;
}




/*Funzione ausiliaria. Prende un processo p, crea un PCB e lo mette nella
lista corretta*/
void FakeOS_createProcess(FakeOS* os, FakeProcess* p) {
  //viene invocata all'arrivo di un processo

  //se timer != arrival_time allora errore
  assert(p->arrival_time==os->timer && "time mismatch in creation");

  //controlla che non ci sia un PCB nella lista running avente lo stesso pid
  ListItem* aux=os->running.first;
  while(aux){
    FakePCB* pcb=(FakePCB*)aux;
    assert(pcb->pid!=p->pid && "pid taken");
    aux=aux->next;
  }
  //controlla che non ci sia un PCB nella lista ready avente lo stesso pid
  aux=os->ready.first;
  while(aux){
    FakePCB* pcb=(FakePCB*)aux;
    assert(pcb->pid!=p->pid && "pid taken");
    aux=aux->next;
  }
  //controlla che non ci sia un PCB nella lista waiting avente lo stesso pid
  aux=os->waiting.first;
  while(aux){
    FakePCB* pcb=(FakePCB*)aux;
    assert(pcb->pid!=p->pid && "pid taken");
    aux=aux->next;
  }
  free(aux);



  //alloca il nuovo PCB
  FakePCB* new_pcb=(FakePCB*) malloc(sizeof(FakePCB));
  //inizializza i campi di new_pcb
  new_pcb->list.next=new_pcb->list.prev=0;
  new_pcb->pid=p->pid;  //il nuovo FakePCB è associato al FakeProcess p
  new_pcb->events=p->events;  //la lista degli event di new_pcb è la stessa di p
  new_pcb->durata_burst=0;
  new_pcb->prediction_burst=0;
  new_pcb->calc_pred=0;

  //controlla che la lista degli eventi non sia vuota
  assert(new_pcb->events.first && "process without events");

  //Mette new_pcb nella coda corretta
  ProcessEvent* e=(ProcessEvent*)new_pcb->events.first;  //primo evento del processo
  switch(e->type){
    case CPU:  //primo evento è un CPU burst
      //metto new_pcb in coda alla lista ready
      List_pushBack(&os->ready, (ListItem*) new_pcb);
      break;
    case IO:  //primo evento è un I/O burst
      //metto new_pcb in coda alla lista waiting
      List_pushBack(&os->waiting, (ListItem*) new_pcb);
      break;
    default:
      assert(0 && "illegal resource");
    ;
  }
}






void FakeOS_simStep(FakeOS* os){
  printf("************** TIME: %08d **************\n", os->timer);

  //processi in processes__________________________________________________________

  ListItem* aux=os->processes.first;  //primo processo della lista processes
  //ciclo sui processi (FakeProcess) della lista processes (processi non arrivati)
  while (aux){
    FakeProcess* proc=(FakeProcess*)aux;
    FakeProcess* new_process=0;
    if (proc->arrival_time==os->timer){  //il processo è arrivato
      new_process=proc;  //salva il processo arrivato in new_process
    }

    aux=aux->next;

    if (new_process) {  //il processo è arrivato
      printf("\tArrivato il processo con pid: %d\n", new_process->pid);
      //stacca il processo dalla lista processes
      new_process=(FakeProcess*)List_detach(&os->processes, (ListItem*)new_process);
      //crea il PCB del processo arrivato e lo mette nella lista corretta
      FakeOS_createProcess(os, new_process);
      free(new_process);
    }
  }
  if (os->processes.size){
    printf("\tDimensione lista processes (processi non ancora arrivati): %d\n", os->processes.size);
  }
  printf("\n");

  //processi in waiting__________________________________________________________

  aux=os->waiting.first;  //punta al primo PCB nella lista waiting
  //ciclo sui FakePCB della lista waiting
  while(aux) {
    FakePCB* pcb=(FakePCB*)aux;
    aux=aux->next;

    ProcessEvent* e=(ProcessEvent*) pcb->events.first;  //primo evento (I/O burst)
    printf("\twaiting: processo con pid  %d\n", pcb->pid);
    assert(e->type==IO);  //controlla che il tipo di evento sia I/O burst
    e->duration--;  //decrementa la darata dell'I/O burst
    printf("\t\tremaining time: %d\n",e->duration);  //stampa la durata rimasta

    if (e->duration==0){  //l'evento è terminato
      printf("\t\tI/O burst terminato\n");  //stampa fine dell'I/O burst
      //toglie l'evento dalla lista degli eventi
      List_popFront(&pcb->events);
      free(e);  //libera la memoria occupata dall'evento e
      //toglie il FakePCB dalla lista waiting
      List_detach(&os->waiting, (ListItem*)pcb);

      if (! pcb->events.first) {  //gli eventi sono finiti
        //il processo è terminato
        printf("\t\tPROCESSO %d TERMINATO\n", pcb->pid);
        free(pcb);  //libera il PCB
      }
      else {
        //si gestisce il prossimo evento
        e=(ProcessEvent*) pcb->events.first;
        switch (e->type){
          case CPU:  //il prossimo evento è un CPU burst
            printf("\t\tspostato il processo in ready\n");
            //metto il PCB in coda alla lista ready
            List_pushBack(&os->ready, (ListItem*) pcb);
            break;
          case IO:  //il prossimo evento è un altro I/O burst
            printf("\t\trimesso il processo in waiting\n");
            //rimetto il PCB nella lista waiting, in coda
            List_pushBack(&os->waiting, (ListItem*) pcb);
            break;
        }
      }
    }
  }


  //processi in running____________________________________________________________

  aux=os->running.first;
  if (!aux){  //nessun processo in running
    printf("\tnessun processo in esecuzione\n");
    goto S;
  }
  //ciclo sui FakePCB della lista running
  while(aux){
    FakePCB* pcb=(FakePCB*)aux;
    aux=aux->next;

    //stampa il pid del processo corrente
    printf("\trunning: processo con pid  %d\n", pcb->pid);
    pcb->durata_burst++;

    ProcessEvent* e=(ProcessEvent*) pcb->events.first;  //evento del processo corrente
    assert(e->type==CPU);  //controlla che sia un cpu burst
    e->duration--;  //decrementa la durata del CPU burst
    printf("\t\tremaining time: %d\n",e->duration);  //stampa la durata rimasta

    if (e->duration==0){  //CPU burst è terminato
      printf("\t\tCPU burst terminato\n");
      //toglie l'evento dalla lista degli eventi
      List_popFront(&pcb->events);
      free(e);  //libera la memoria allocata all'evento
      //toglie il FakePCB dalla lista running
      List_detach(&os->running, (ListItem*)pcb);

      if (! pcb->events.first) {  //gli eventi sono finiti
        //il processo è terminato
        printf("\t\tPROCESSO %d TERMINATO\n", pcb->pid);
        free(pcb); //libera il PCB
      }
      else {
        //si gestisce il prossimo evento
        e=(ProcessEvent*) pcb->events.first;
        switch (e->type){
          case CPU:  //il prossimo evento è un CPU burst
            printf("\t\tspostato il processo in ready\n");
            //mette il FakePCB in coda alla lista ready
            List_pushBack(&os->ready, (ListItem*) pcb);
            break;
          case IO:  //il prossimo evento è un I/O burst
            printf("\t\tspostato il processo in waiting\n");
            //mette il FakePCB in coda alla lista waiting
            List_pushBack(&os->waiting, (ListItem*) pcb);
            break;
        }
      }
    }
  }

S:
  printf("\n\tDimensione lista running: %d\n", os->running.size);
  printf("\tDimensione lista waiting: %d\n", os->waiting.size);
  printf("\tDimensione lista ready: %d\n\n", os->ready.size);

  //scheduling_____________________________________________________________________

  if (os->schedule_fn){
    //invoca la funzione di scheduling
    (*os->schedule_fn)(os, os->schedule_args);
  }


  ++os->timer;  //incrementa il timer
  printf("\n");
}


