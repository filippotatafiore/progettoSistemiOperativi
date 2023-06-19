#pragma once

typedef struct ListItem {  //elementi della lista
  struct ListItem* prev;
  struct ListItem* next;
} ListItem;

typedef struct ListHead {  //lista
  ListItem* first;  //primo elemento della lista
  ListItem* last;  //ultimo elemento della lista
  int size;  //dimensione lista
} ListHead;

//inizializza lista, con head puntatore al primo elemento
void List_init(ListHead* head);

//trova item nella lista head
ListItem* List_find(ListHead* head, ListItem* item);

//inserisce item nella lista head, dopo previous
ListItem* List_insert(ListHead* head, ListItem* previous, ListItem* item);

//rimuove item dalla lista head
ListItem* List_detach(ListHead* head, ListItem* item);

//inserisce item alla fine della lista head
ListItem* List_pushBack(ListHead* head, ListItem* item);

//inserisce item in testa alla lista head
ListItem* List_pushFront(ListHead* head, ListItem* item);

//rimuove l'elemento in testa alla lista head
ListItem* List_popFront(ListHead* head);

