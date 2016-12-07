CLAGS=-g -O2 
PFLAGS=-lsimlib -lm
AUTORI= xnodza00_xmolek00
SOUBORY= Makefile sim.cpp dokumentace.pdf 
CC = g++

all: sim

sim: sim.cpp
	$(CC) $(CFLAGS) -o $@ sim.cpp $(PFLAGS)


run:
	./sim 14 6 exp1.out		#zjisteni vytizeni pokladen
	./sim 11 6 exp2.out		#experiment nezvladatelnosti

	./sim 14 4 exp3.out		#experimenty na optimalizaci
	./sim 14 5 exp4.out
	./sim 14 6 exp5.out
	./sim 14 7 exp6.out
	./sim 14 8 exp7.out
	./sim 14 9 exp8.out
	./sim 14 10 exp9.out

zip:
	zip 04_$(AUTORI).zip $(SOUBORY)

runMy: 	
	./sim 14 4 experiment1.out	
	./sim 14 5 experiment2.out
	./sim 14 6 experiment3.out
	./sim 14 7 experiment4.out
	./sim 14 8 experiment5.out
	./sim 14 9 experiment6.out
	./sim 14 10 experiment7.out

	./sim 11 6 experiment8.out
	./sim 12 6 experiment9.out
	./sim 13 6 experiment10.out
	./sim 14 6 experiment11.out
	./sim 15 6 experiment12.out
	./sim 16 6 experiment13.out
	./sim 17 6 experiment14.out
	./sim 18 6 experiment15.out
	./sim 19 6 experiment16.out
	./sim 20 6 experiment17.out
