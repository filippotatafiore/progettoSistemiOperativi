CC=gcc
CCOPTS=--std=gnu99 -Wall -D_LIST_DEBUG_
AR=ar

OBJS=linked_list.o fake_process.o fake_os.o

HEADERS=linked_list.h  fake_process.h

BINS = sched_sim

.phony: clean all


all:	$(BINS)

#trasforma un file .c in un file .o
%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

#crea il file eseguibile sched_sim
sched_sim:	sched_sim.c $(OBJS)
	$(CC) $(CCOPTS) -o $@ $^

#elimina i file .o e l'eseguibile
clean:
	rm -rf *.o *~ $(OBJS) $(BINS)





