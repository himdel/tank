CC = gcc
CFLAGS = -pedantic -Wall -std=c99 -Wextra -g
LIBS = -lm -g
SDLLIBS = `sdl-config --libs`
SDLCFLAGS = `sdl-config --cflags`

OBJS = general.o letters.o options.o water.o paint.o


all: tank

tank: main.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(SDLLIBS)

paint.o: paint.c paint.h general.h
	$(CC) -o $@ -c $< $(CFLAGS) $(SDLCFLAGS) -DTANK

main.o: main.c paint.h letters.h options.h water.h general.h
	$(CC) -o $@ -c $< $(CFLAGS) $(SDLCFLAGS)

general.o: general.c general.h
	$(CC) -o $@ -c $< $(CFLAGS)

letters.o: letters.c letters.h let_fnt.h paint.h general.h
	$(CC) -o $@ -c $< $(CFLAGS)

options.o: options.c options.h general.h opt_save opt_load
	$(CC) -o $@ -c $< $(CFLAGS) -DLINUX

water.o: water.c water.h options.h paint.h general.h
	$(CC) -o $@ -c $< $(CFLAGS)


opt_save: options.h scripts/gensave.pl
	perl scripts/gensave.pl < $< > $@

opt_load: options.h scripts/genload.pl
	perl scripts/genload.pl < $< > $@


let_fnt: let_fnt.c let_fnt.h
	$(CC) -o $@ $< $(CFLAGS)

let_fnt.xpm: let_fnt
	./$< > $@

let_fnt.png: let_fnt.xpm
	convert $< $@


clean:
	rm -f tank opt_save opt_load *.o *~ let_fnt.png let_fnt.xpm let_fnt core
