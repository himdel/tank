#include <stdio.h>
#include <stdlib.h>
#include "let_fnt.h"

#define FONT_W 8
#define FONT_H 16


int
pwr(int a, int b)
{
	int c = 1, d;

	for (d = 0; d < b; d++)
		c *= a;

	return c;
}


void
wrtltr(char *a, int x, int y, int c)
{
	for (int yy = 0; yy < FONT_H; yy++)
		for (int xx = 0; xx < FONT_W; xx++)
			a[(y + yy) * (32 * FONT_W) + x + xx] = ( font_default[( c * FONT_H ) + yy] & pwr( 2, 7 - xx ) ) ? 'x' : 'o';
}


int
main()
{
	printf("/* XPM */\nstatic char * XFACE[] = {\n\"%d %d %d %d\",\n\"o c #000000\",\n\"x c #ffffff\",\n", 32 * FONT_W, 8 * FONT_H, 2, 1);
	char *arr = malloc(32 * FONT_W * 8 * FONT_H * sizeof(char));

	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 32; i++)
			wrtltr(arr, i * FONT_W, j * FONT_H, j * 32 + i);

	for (int j = 0; j < 8 * FONT_H; j++) {
		printf("\"");
		for (int i = 0; i < 32 * FONT_W; i++)
			printf("%c", arr[j * (32 * FONT_W) + i]);
		printf("\"%s\n", (j == (8 * FONT_H) - 1) ? "};" : ",");
	}

	return 0;
}
