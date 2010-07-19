/*
  water.c - water routines for tank
  himdel@seznam.cz
  
  the formula is: p = g * r * h
  p - pressure (Pa = N / m^2)
  g - gravitational acceleration (= 10  N / kg)
  r - density of the liquid (= 1000  kg / m^3)
  h - depth
 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "paint.h"
#include "water.h"
#include "options.h"
#include "general.h"


#ifndef PI
#define PI M_PI
#endif

int gwh (int, int);
int swh (int, int);
void rwh (int, int);

struct pts
  {
    int x, y;
    struct pts *n, *p;
  };
 
int wtr[640];    /* water levels */
int *wtrh;     /* water pressures */



void
water_init ()
{
  int foo;
  
  printf ("water_init: .");
  printf (".");
  wtrh = (int *) calloc (640 * 480, sizeof (int));
  printf (".");
  for (foo = 0; foo < (640 * 480); foo++)
    wtrh[foo] = -1;
  printf (" OK\n");
}


void
water_destroy ()
{
  if (wtrh != NULL)
    free (wtrh);
}



void
water_land (lmhs)
   int *lmhs;
{
  struct pts *lpt, *lpb, *hpt, *hpb;

  int foo, bar;

  for (foo = 0; foo < 640; foo++)
    wtr[foo] = 0;
  
  lpb = lpt = (struct pts *) malloc (sizeof (struct pts));
  if (lpt == NULL)
    return;
  lpt->y = 0;
  lpt->n = NULL;
  
  hpb = hpt = (struct pts *) malloc (sizeof (struct pts));
  if (hpt == NULL)
    {
      free (lpt);
      return;
    }
  hpt->y = 480;
  hpt->n = NULL;
  

/* find lowpoints and highpoints : */

  bar = lmhs[1];
  
  for (foo = 0; foo < 640; foo++)    
    {
      if (bar < lmhs[foo])
        {
          if (lmhs[foo] > lpt->y)
            {
              lpt->x = foo;
              lpt->y = lmhs[foo];
            }
          else
            {
              lpt->n = (struct pts *) malloc (sizeof (struct pts));
              lpt = lpt->n;
              if (lpt != NULL)
                {
                  lpt->x = 640;
                  lpt->y = 0;
                  lpt->n = NULL;
                }
            }
        }

      if (bar > lmhs[foo])
        {
          if (lmhs[foo] < hpt->y)
            {
              hpt->x = foo;
              hpt->y = lmhs[foo];
            }
          else
            {
              hpt->n = (struct pts *) malloc (sizeof (struct pts));
              hpt = hpt->n;
              if (hpt != NULL)
                {
                  hpt->y = 480;
                  hpt->n = NULL;
                }
            }
        }

      if ((lpt == NULL) || (hpt == NULL))
        {
          lpt = lpb;
          hpt = hpb;
          
          while (lpt != NULL)
            {
              lpb = lpt;
              lpt = lpt->n;
              free (lpb);
            }

          while (hpt != NULL)
            {
              hpb = hpt;
              hpt = hpt->n;
              free (hpb);
            }
        }
        
      bar = lmhs[foo];
    }
  
  
/* find valleys and foreach: water level = ((3 * (min(hp0, hp1) - lp)) / 4) */

  lpt = lpb;
  hpt = hpb;
  
  while ((lpt != NULL) && (hpt != NULL))
    {
      int h0, h0x, h1, h1x, l;

      if (hpt->x < lpt->x)
        {
          h0 = hpt->y;
          h0x = hpt->x;
          hpb = hpt;
          hpt = hpt->n;
          free (hpb);
        }
      else
        {
          h0 = 0;
          h0x = 0;
        }

      if (hpt != NULL)
        {
          h1 = hpt->y;
          h1x = hpt->x;
        }        
      else
        {
          h1 = 0;
          h1x = 639;
        }
        
      l = (3 * (lpt->y - max (h0, h1))) / 4;
      if (opt_v)
        printf ("valley: center (%d, %d); level = %d; h0 (%d, %d); h1 (%d, %d)\n", lpt->x, lpt->y, l, h0x, h0, h1x,h1);
      l = lpt->y - l;

      for (foo = h0x; (foo < h1x) && (foo < 640); foo++)
        if (lmhs[foo] > l)
          wtr[foo] = l;

      lpb = lpt;
      lpt = lpt->n;
      free (lpb);
    }
  
  
/* paint: */

  for (foo = 0; foo < 640; foo++)
    if (wtr[foo])
      {
        int bar;
        him_line (foo, wtr[foo], foo, lmhs[foo], opt_col_wtr, 1);
        for (bar = 0; bar < (lmhs[foo] - wtr[foo]); bar++)
          wtrh[(bar * 640) + foo] = bar;
      }
}


void
rewater ()
{
  int x, y;
  
  if (!opt_rewater)
    return;
    
  for (x = 0; x < 640; x++)
    if (wtr[x])
      for (y = wtr[x]; y < 480; y++)
        {
          int c;

          if ((x > 0) && (him_getpixel (x - 1, y, 1) == -1) && (gwh (x, y) >= 0))
            {
              swh (x - 1, y);
              rwh (x, wtr[x]);
            }

          if ((x < 639) && (him_getpixel (x + 1, y, 1) == -1) && (gwh (x, y) >= 0))
            {
              swh (x + 1, y);
              rwh (x, wtr[x]);
            }
          
          if ((c = him_getpixel (x, y, 1)) == -1)
            {
              swh (x, y);
              rwh (x, wtr[x]);
            }
          else if (c == opt_col_land)
            break;
        }
}


int
getwl (x)
    int x;
{
  return wtr[x];
}


int
gwh (x, y)
   int x, y;
{
  if ((y < 0) || (x < 0) || (x > 639) || (y > 479))
    return -1;
  return wtrh[(y * 640) + x];
}


int
swh (x, y)
   int x, y;
{
  him_pixel (x, y, opt_col_wtr, 1);

  if ((wtr[x] > y) || (wtr[x] == 0))
    {
      int c = 0;
      wtr[x] = y;
      wtrh[(y * 640) + x] = 0;
      y++;
      while ((y < 480) && (wtrh[(y * 640) + x] >= 0))
        wtrh[(y++ * 640) + x] = ++c;
    }
  else 
    wtrh[(y * 640) + x] = y - wtr[x];
  
  return wtrh[(y * 640) + x];
}


void
rwh (x, y)
   int x, y;
{
  int c = -1;
  
  if (gwh (x, y) == -1)
    return;
  
  if (wtr[x] == y)
    wtr[x] = y + 1;
    
  wtrh[(y * 640) + x] = -1;
  him_pixel (x, y++, -1, 1);

  while ((y < 480) && (wtrh[(y * 640) + x] >= 0))
    wtrh[(y++ * 640) + x] = c++;  

/*  while ((gwh (x, wtr[x]) == -1) && (wtr[x] < 480))
    wtr[x] += 1;   --DOESNT SEEM TO HELP */
}


void
wtr_hole (x, y)
    int x, y;
{
  rwh (x, y);
}
