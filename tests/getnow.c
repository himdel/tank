#include <stdio.h>
#include "../paint.h"

int
main()
{
	unsigned int foo, bar;
	int x, y;
	
	foo = him_getnow();
	for (x = 0; x < 262144; x++)
		y += x * x; 
	
	bar = him_getnow();
	printf ("bar - foo = %d (y = %d)\n", bar - foo, y);
	exit(0);
}