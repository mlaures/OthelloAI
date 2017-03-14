CC          = g++
CFLAGS      = -std=c++11 -Wall -pedantic -ggdb -lpthread
OBJS        = player.o board.o 
PLAYERNAME  = icantdecide

all: $(PLAYERNAME) testgame

$(PLAYERNAME): $(OBJS) wrapper.o
	$(CC) -o $@ $^ $(CFLAGS)

$(PLAYERNAME): player.hpp

simulator: $(OBJS) PlayerEvolutionSimulator.o
	$(CC) -o $@ $^ $(CFLAGS)

testgame: testgame.o
	$(CC) -o $@ $^ $(CFLAGS)

testminimax: $(OBJS) testminimax.o
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) -x c++ $< -o $@

java:
	make -C java/

cleanjava:
	make -C java/ clean

clean:
	rm -f *.o $(PLAYERNAME) testgame testminimax simulator

.PHONY: java testminimax
