#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fake_os.h"

FakeOS os;

typedef struct {  //argomenti da passare allo scheduler
  int quantum;
  float decay_coeff;  //coefficiente per calcolare le predizioni
  int num_cpus;  //numero di cpu
} SchedArgs;



//funzione di scheduling
void schedule(FakeOS* os, void* args_){
  //salva in args la struct degli argomenti passati allo scheduler
  SchedArgs* args=(SchedArgs*)args_;

  //numero massimo di processi che posso inserire in running
  int diff=(args->num_cpus)-(os->running.size);

  if (! os->ready.first){  //lista ready vuota
    printf("\tnon ci sono processi da inserire in running\n");
    return;
  }


  /*calcoliamo per ogni processo in ready la prediction, la salviamo nel campo apposito,
  e salviamo in min i valori delle prediction più piccole*/
  float min[diff];
  for (int i=0; i<diff; i++){
    min[i]=-1;  //vuoto
  }
  ListItem* aux=os->ready.first;
  while(aux){  //ciclo sui FakePCB in ready
    FakePCB* pcb=(FakePCB*)aux;
    aux=aux->next;

    /*usiamo la variabile ausiliaria calc_pred per controllare se è già stata calcolata la
    prediction per il processo*/
    if (pcb->calc_pred==0){
      //calcola la prediction e la salva in prediction_burst
      float p=(args->decay_coeff)*(pcb->durata_burst)+(1-(args->decay_coeff))*(pcb->prediction_burst);
      pcb->prediction_burst=p;
      pcb->calc_pred=1;
    }
    
    /*  m: valore più grande in min
        c: indice di m
        j: indice del primo valore -1 trovato in min (se presente)
    */
    float m=-2;
    int c=-1;
    int j=-1;
    for (int i=0; i<diff; i++){
      if (min[i]==-1 && j==-1){
        j=i;
      }
      if (min[i]>m){
          m=min[i];
          c=i;
      }
    }
    //se si è trovato un valore -1 in min (vuoto), vi si mette la prediction
    if (j!=-1){
      min[j]=pcb->prediction_burst;
    }
    //se la prediction è minore del più grande elemento in min, la mettiamo in min
    else if (pcb->prediction_burst < m){
      min[c]=pcb->prediction_burst;
    }
  }
  
  /*ora raccatto in ready i processi i cui valori della prediction ho salvato in min, 
  e li metto in running: per ogni elemento in min cerco in ready un elemento che abbia
  prediction con lo stesso valore*/
  for (int i=0; i<diff; i++){  //ciclo su min
    aux=os->ready.first;
    while(aux){  //ciclo su ready
      FakePCB* pcb=(FakePCB*)aux;
      aux=aux->next;

      if (pcb->prediction_burst==min[i]){
        //rimuovo pcb da ready e lo inserisco in running
        List_pushBack(&os->running, List_detach(&os->ready, (ListItem*) pcb));
        pcb->calc_pred=0;
        pcb->durata_burst=0;
        printf("\tRimosso il processo con pid %d da ready e inserito in running\n", pcb->pid);

        assert(pcb->events.first);  //controlla che la lista degli eventi non sia vuota
        ProcessEvent* e = (ProcessEvent*)pcb->events.first;  //primo evento del nuovo PCB
        assert(e->type==CPU);  //controlla che sia di tipo CPU

        if (e->duration > args->quantum) {  //se la durata del CPU burst è maggiore del quantum
          //creo un nuovo evento qe CPU burst
          ProcessEvent* qe=(ProcessEvent*)malloc(sizeof(ProcessEvent));
          qe->list.prev=qe->list.next=0;
          qe->type=CPU;
          qe->duration=args->quantum;  //setto la durata di qe a quantum
          e->duration-=args->quantum;  //sottraggo alla durata di e la durata di quantum
          //metto in testa alla lista degli eventi del nuovo PCB corrente l'evento qe
          List_pushFront(&pcb->events, (ListItem*)qe);
        }

        break;
      }
    }
  }
}







int main(int argc, char** argv) {
  FakeOS_init(&os);  //inizializza i campi di os
  SchedArgs s_args;  //definisce la struct SchedArgs
  
  int c, q;
  float d;
  int z=0;
  z+=sscanf(argv[1], "%d", &c);  //legge il numero delle cpu
  z+=sscanf(argv[2], "%d", &q);  //legge il quantum
  z+=sscanf(argv[3], "%f", &d);  //legge il decay coefficient

  if (argc<=3 || z<3){
    printf("Parametri da passare al terminale (in ordine):\n\tnumero delle cpu\tquantum\t\tdecay coefficient\tfile .txt\n");
    return 0;
  }
  
  //inizializza i campi di s_args
  s_args.num_cpus=c;  //setta il numero di cpu
  s_args.quantum=q;  //setta il quantum
  s_args.decay_coeff=d;  //setta il decay coefficient
  assert(s_args.decay_coeff >= 0 && s_args.decay_coeff <= 1);

  os.schedule_args=&s_args;
  os.schedule_fn=schedule;
  
  for (int i=4; i<argc; i++){  //file passati
    FakeProcess new_process;
    //carica le informazioni del file argv[i] in new_process
    int num_events=FakeProcess_load(&new_process, argv[i]);
    printf("loading [%s], pid: %d, events:%d\n", argv[i], new_process.pid, num_events);

    if (num_events) {  //se il processo caricato ha degli eventi
      FakeProcess* new_process_ptr=(FakeProcess*)malloc(sizeof(FakeProcess));
      *new_process_ptr=new_process;  //puntatore al processo appena creato
      //mette new_process in coda alla lista processes
      List_pushBack(&os.processes, (ListItem*)new_process_ptr);
    }
  }

  printf("\nnumero processi caricati: %d\n", os.processes.size);
  
  printf("quantum: %d\n", s_args.quantum);
  printf("numero cpu: %d\n", s_args.num_cpus);
  printf("decay coefficient: %f\n\n", s_args.decay_coeff);

  //viene chiamata la funzione FakeOS_simStep ("giro di giostra") finché c'è un processo in running, o c'è un
  //processo in una delle liste
  while(os.running.first || os.ready.first || os.waiting.first || os.processes.first){
    FakeOS_simStep(&os);
  }

  printf("********************************************\n\n");
}

