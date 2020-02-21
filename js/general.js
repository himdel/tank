/*
  general.c - general routines for fonted
 */


int pwr (int, int);
char *itoaH (int);
void reverse (char *, int);
void getstr (char *, int);
int roundH (double);
double sqr (double);
double near (int, int, int, int);
int max (int, int);
int min (int, int);
int hgetline (FILE *, char *, int);
/*
  general.c - general routines for fonted
 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "general.h"


int
pwr (a, b)
    int a, b;
{
  int c = 1, d;

  for (d = 0; d < b; d++)
    c *= a; 

  return (c);
}


char *
itoaH (i)
     int i;
{
  int sign, foo = 0;
  char *s;

  s = (char *) malloc (16);
  if (s == NULL)
    return (s);

  sign = (i > 0) ? 1 : -1;
  i *= sign;

  while (i)
    {
      s[foo++] = (i % 10) + '0';
      i /= 10;
    }

  if (sign == -1)
    {
      s[foo] = (foo == 0) ? '0' : '-';
      foo++;
    }

  s[foo] = 0;

  reverse (s, foo);
  return (s); 
}


void
reverse (s, l)
     char *s;
     int l;
{
  int foo;
  for (foo = 0; foo < (l / 2); foo ++)
    {
      char z;
      z = s[foo];
      s[foo] = s[l - 1 - foo];
      s[l - 1 - foo] = z;
    }
}


void
getstr (s, l)
     char *s;
     int l;
{
  int c;
  while (((c = getchar ()) != EOF) && (c != '\n') && l--)
    *(s++) = c;
  *s = 0;
}


int
roundH (f)
    float f;
{
  return ((f < (((int) f) + 0.5)) ? ((int) f) : (((int) f) + 1));
}


double
sqr (n)
    double n;
{
  return (n * n);
}


double
near (x1, y1, x2, y2)
    int x1, y1, x2, y2;
{
  int dx, dy;

  dx = abs (x2 - x1);
  dy = abs (y2 - y1);

  return sqrt (sqr ((double) dx) + sqr ((double) dy));
}


int
max (a, b)
    int a, b;
{
  return ((a > b) ? a : b);
}


int
min (a, b)
    int a, b;
{
  return ((a < b) ? a : b);
}


int
hgetline (fp, s, max)
    FILE *fp;
    char *s;
    int max;
{
  char c;

  s[max] = 0;
  c = fgetc (fp);
  while ((c != EOF) && (c != '\n'))
    {
      if (max--)
        *(s++) = c;
      c = fgetc (fp);
    }
  *s = 0;

  return c;
}
