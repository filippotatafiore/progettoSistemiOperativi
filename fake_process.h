#pragma once
#include "linked_list.h"

typedef enum {CPU=0, IO=1} ResourceType;

//evento di un processo (CPU burst o I/O burst)
typedef struct {
  ListItem list;  //un evento farà parte di una lista
  ResourceType type;  //tipo dell'evento (CPU o I/O burst)
  int duration;  //durata evento
} ProcessEvent;

//processo
typedef struct {
  ListItem list;  //un processo farà parte di una lista
  int pid; //pid del processo
  int arrival_time;  //tempo di arrivo del processo
  ListHead events;  //lista degli eventi del processo (lista di ProcessEvent)
} FakeProcess;


/*carica le informazioni di un processo ricavate dal file "filename" nel FakeProcess p.
Per ogni evento crea un ProcessEvent che salva nella lista events del FakeProcess.
Restituisce il numero di eventi (CPU burst o I/O burst) del processo*/
int FakeProcess_load(FakeProcess* p, const char* filename);

/*scrive sul file "filename" le informazioni riguardanti il FakeProcess p.
Restituisce il numero di eventi del processo*/
int FakeProcess_save(const FakeProcess* p, const char* filename);

