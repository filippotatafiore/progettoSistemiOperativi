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

#crea il file eseguibile a partire dal file sched_sim.c e dai file .o creati
sched_sim:	sched_sim.c $(OBJS)
	$(CC) $(CCOPTS) -o $@ $^

#elimina file .o ed eseguibile
clean:
	rm -rf *.o *~ $(OBJS) $(BINS)





