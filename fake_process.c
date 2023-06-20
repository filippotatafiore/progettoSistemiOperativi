#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_process.h"

#define LINE_LENGTH 1024

int FakeProcess_load(FakeProcess* p, const char* filename) {
  FILE* f=fopen(filename, "r");  //apre il file in lettura
  assert(f);

  // read the PID
  char *buffer=NULL;
  size_t line_length=0;

  //iniziallizzazione campi di p
  p->pid=-1;
  p->arrival_time=-1;
  List_init(&p->events);
  p->list.prev=p->list.next=0;

  int num_events=0;  //contatore del numero di eventi del processo

  while (getline(&buffer, &line_length, f) >0){
    //letta una riga e salvata in buffer (salva la dimensione in &line_length)
    int pid=-1;
    int arrival_time=-1;
    int num_tokens=0;
    int duration=-1;

    /*prova a leggere un token della riga nel formato "PROCESS %d %d" (prima riga del file).
    Salva il primo intero letto in pid, e il secondo in arrival_time*/
    num_tokens=sscanf(buffer, "PROCESS %d %d", &pid, &arrival_time);

    if (num_tokens==2 && p->pid<0){  //ha letto, correttamente, la prima riga
      p->pid=pid;  //salva il pid trovato in p->pid
      p->arrival_time=arrival_time;  //salva il tempo di arrivo trovato in p->arrival_time
      goto next_round;
    }

    /*prova a leggere un token della riga nel formato "CPU_BURST %d", salvando in duration
    l'intero letto*/
    num_tokens=sscanf(buffer, "CPU_BURST %d", &duration);

    if (num_tokens==1){  //ha letto, correttamente, una riga che descrive un CPU burst
      //crea un ProcessEvent
      ProcessEvent* e=(ProcessEvent*) malloc(sizeof(ProcessEvent));
      e->list.prev=e->list.next=0;  //inizializza il campo list di ProcessEvent
      e->type=CPU;  //è un CPU burst
      e->duration=duration;  //salva la durata dell'evento
      //mette l'evento creato in coda alla lista degli eventi di p
      List_pushBack(&p->events, (ListItem*)e);
      ++num_events;
      goto next_round;
    }

    /*prova a leggere un token della riga nel formato "IO_BURST %d", salvando in duration
    l'intero letto*/
    num_tokens=sscanf(buffer, "IO_BURST %d", &duration);

    if (num_tokens==1){  //ha letto, correttamente, una riga che descrive un I/O burst
      //crea un PorcessEvent
      ProcessEvent* e=(ProcessEvent*) malloc(sizeof(ProcessEvent));
      e->list.prev=e->list.next=0;  //inizializza il campo list di ProcessEvent
      e->type=IO;  //è un I/O burst
      e->duration=duration;  //salva la durata dell'evento
      //mette l'evento creato in coda alla lista degli eventi di p
      List_pushBack(&p->events, (ListItem*)e);
      ++num_events;
      goto next_round;
    }

    next_round:;
  }
  /*Alla fine del ciclo avrà riempito i campi del FakeProcess p, fra cui la lista di
  ProcessEvent*/


  if (buffer)
    free(buffer);  //dealloca buffer

  fclose(f);  //chiude il file f

  return num_events;  //restituisce il numero di eventi
}






int FakeProcess_save(const FakeProcess* p, const char* filename){
  FILE* f=fopen(filename, "w");  //apre il file in scrittura
  assert(f);

  fprintf(f, "PROCESS %d %d\n", p->pid, p->arrival_time);  //scrive la prima riga

  ListItem* aux=p->events.first;  //puntatore al primo ProcessEvent
  int num_events = 0;
  while(aux) {  //ciclo che scorre tutti i ProcessEvet nella lista degli eventi
    ProcessEvent* e=(ProcessEvent*) aux;  //ProcessEvent corrente
    switch(e->type){
      case CPU:  //CPU burst
        //scrive sul file la riga corrispondente al CPU burst
        fprintf(f, "CPU_BURST %d\n", e->duration);
        ++ num_events;
        break;
      case IO:  //I/O burst
        //scrive sul file la riga corrispondente all'I/O burst
        fprintf(f, "IO_BURST %d\n", e->duration);
        ++ num_events;
        break;
      default:;
    }

    aux=aux->next;
  }
  /*alla fine del ciclo avrà scritto nel file tutti gli eventi*/

  fclose(f);  //chiude il file

  return num_events;  //restituisce il numero di eventi
}


