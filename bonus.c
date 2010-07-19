/*
  bonus.c - bonus routines for tank
  himdel@seznam.cz
 */

#include <stdio.h>
#include <stdlib.h>

#include "bonus.h"
#include "options.h"
#include "paint.h"
#include "general.h"


struct bnss 
  {
    int x, y, t;
  } *bns;

int bn;



int
bns_init (n)
   int n;
{
  if (n == 0)
    return (bn = 0);
    
  bns = (struct bnss *) calloc (bn = n, sizeof (struct bnss));
  if (bns == NULL)
    return -1;
  else
    return 0;
}


void
bns_destroy ()
{
  if (bns != NULL)
    free (bns);
}



void 
bns_gen (lmhs)
   int *lmhs; 
{
  int foo;
  for (foo = 0; foo < bn; foo++)
    {
      bns[foo].x = rand () % 640;
      bns[foo].y = rand () % lmhs[bns[foo].x];
      bns[foo].t = rand () % opt_num_bt;
      him_ufullcircle (bns[foo].x, bns[foo].y, 6, opt_col_bns, 4, &him_pixel);
    }
}


void
bns_hit (x, y)
    int x, y;
{
  int foo, a;
  unsigned int b;
  
  a = 0; b = ~0;
  
  for (foo = 0; foo < bn; foo++)
    {
      int bar = near (x, y, bns[foo].x, bns[foo].y);
      if (bar < b)
        {
          a = foo;
          b = bar;
        } 
    }

  /* TODO process:
  switch (bns[a].t)
    {
      default:;
    }
   */
  

  bns[a].t = -1;
  him_ufullcircle (bns[a].x, bns[a].y, 8, -1, 4, &him_pixel);
}
