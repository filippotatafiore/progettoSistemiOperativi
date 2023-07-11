#include "fake_process.h"
#include "linked_list.h"
#pragma once


typedef struct {  //PCB
  ListItem list;  //Un PCB farà parte di una lista (ready, waiting, ...)
  int pid;  //pid del processo associato
  ListHead events;  //lista di eventi (CPU burst e I/O burst) del processo
  int durata_burst;  //durata del cpu burst eseguito
  float prediction_burst;  //predizione del cpu burst
  int calc_pred;  //variabile ausiliaria
} FakePCB;


struct FakeOS;
//definisco ScheduleFn, puntatore a una funzione void che prende gli argomenti os e args
typedef void (*ScheduleFn)(struct FakeOS* os, void* args);


typedef struct FakeOS{
  ListHead running;  //lista dei processi in esecuzione (lista di FakePCB)
  ListHead ready;  //lista ready (lista di FakePCB)
  ListHead waiting;  //lista waiting (lista di FakePCB)
  int timer;  //intero che usiamo per contare
  ScheduleFn schedule_fn;  //puntatore a una funzione di scheduling
  void* schedule_args;  //argomenti da passare alla funzione di scheduling
  ListHead processes;  //lista di tutti i processi (lista di FakeProcess)
} FakeOS;

//costruttore, inizializza i campi os
void FakeOS_init(FakeOS* os);

//fa "girare la giostra"
void FakeOS_simStep(FakeOS* os);


