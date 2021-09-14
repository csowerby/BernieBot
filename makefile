
CC = gcc
CFLAGS=

all:
	$(CC) $(CFLAGS) src/main.c src/init.c src/engine.c src/perft.c src/gamestate.c -o BernieBot
	./BernieBot
	make clean


init.o: init.c defs.h
	$(CC) $(CFLAGS) -c init.c

GameState.o: GameState.c GameState.h
	 $(CC) $(CFLAGS) -c GameState.c

engine.o: engine.c engine.h
	$(CC) $(CFLAGS) -c engine.c

Perft.o: Perft.c Perft.h

BernieBot: init.o GameState.o

#--------------------
fast:
	$(CC) -O3 $(CFLAGS) main.c init.c engine.c Perft.c GameState.c -o BernieBot

assembly:
	$(CC) -O0 -S main.c engine.c main.c init.c engine.c Perft.c GameState.c


clean:
	rm -r -f *.s *.o BernieBot
