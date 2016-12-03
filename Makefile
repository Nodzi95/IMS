CLAGS=-g -O2 
PFLAGS=-lsimlib -lm
 
CC = g++

all: sim

sim: sim.cpp
	$(CC) $(CFLAGS) -o $@ sim.cpp $(PFLAGS)

rebuild: clean all
