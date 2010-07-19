CC = gcc
CFLAGS = -pedantic -Wall
LIBS = -lm
SDLLIBS = `sdl-config --libs` -lSDL_image
SDLCFLAGS = `sdl-config --cflags` -ansi
PERL = perl
VERS = 0.8a

SRC = *.c *.h README INSTALL TODO ChangeLog Makefile icon.bmp *.png scripts/*.pl
OBJS = general.o letters.o options.o water.o bonus.o menu.o gui.o weapons.o lightning.o shots.o expls.o


all: tank tankSDL

tank: main.o paint.o $(OBJS)
	$(CC) -o $@ $^ -lvga $(LIBS)

tankSDL: main.o paintSDL.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(SDLLIBS)

main.o: main.c paint.h letters.h options.h water.h bonus.h menu.h
	$(CC) -o $@ -c $< $(CFLAGS)

general.o: general.c general.h
	$(CC) -o $@ -c $< $(CFLAGS)

paint.o: paint.c paint.h general.h options.h
	$(CC) -o $@ -c $< $(CFLAGS) -DTANK

paintSDL.o: paintSDL.c paint.h general.h options.h
	$(CC) -o $@ -c $< $(CFLAGS) $(SDLCFLAGS) -DTANK

letters.o: letters.c letters.h let_fnt.h paint.h general.h
	$(CC) -o $@ -c $< $(CFLAGS)

options.o: options.c options.h general.h opt_save opt_load
	$(CC) -o $@ -c $< $(CFLAGS) -DLINUX

water.o: water.c water.h options.h paint.h general.h
	$(CC) -o $@ -c $< $(CFLAGS)

bonus.o: bonus.c bonus.h options.h paint.h general.h
	$(CC) -o $@ -c $< $(CFLAGS)

menu.o: menu.c menu.h paint.h options.h letters.h general.h gui.h
	$(CC) -o $@ -c $< $(CFLAGS)

opt_save: options.h
	$(PERL) scripts/gensave.pl < $< > $@

opt_load: options.h
	$(PERL) scripts/genload.pl < $< > $@
	echo "      else if (strlen (fn) > 1)" >> $@
	echo "        printf (\"tank: Unknown option %s -IGNORING\n\", fn);" >> $@

gui.o: gui.c gui.h paint.h letters.h general.h
	$(CC) -o $@ -c $< $(CFLAGS)

expls.o: expls.c expls.h paint.h general.h options.h
	$(CC) -o $@ -c $< $(CFLAGS)

shots.o: shots.c shots.h paint.h general.h options.h
	$(CC) -o $@ -c $< $(CFLAGS)

lightning.o: lightning.c lightning.h paint.h general.h options.h
	$(CC) -o $@ -c $< $(CFLAGS)

weapons.o: weapons.c weapons.h paint.h general.h options.h
	$(CC) -o $@ -c $< $(CFLAGS)


let_fnt: let_fnt.c let_fnt.h
	$(CC) -o $@ $< $(CFLAGS) -std=c99

let_fnt.xpm: let_fnt
	./$< > $@

let_fnt.png: let_fnt.xpm
	convert $< $@

clean:
	rm -f tank tankSDL opt_save opt_load *.o *~ let_fnt.png let_fnt.xpm let_fnt


tank-$(VERS).tar.gz:
	@ls $(SRC) | sed s:^:tank-$(VERS)/: >MANIFEST
	@(cd ..; ln -s tank tank-$(VERS))
	(cd ..; tar -cvzf tank/tank-$(VERS).tar.gz `cat tank/MANIFEST`)
	@(cd ..; rm tank-$(VERS))
