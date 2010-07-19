FLAGS=../general.o ../options.o -lvga -lm
PAINT=../paint.o

all: circfr colvals getnow


circfr: circfr.c $(PAINT)
	gcc -o $@ $^ $(FLAGS)

colvals: colvals.c $(PAINT)
	gcc -o $@ $^ $(FLAGS)

getnow: getnow.c $(PAINT)
	gcc -o $@ $^ $(FLAGS)


clean: 
	rm -f circfr colvals getnow *~
