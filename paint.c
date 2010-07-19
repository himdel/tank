/*
  paint.c - him_painting routines for tank
  himdel@seznam.cz
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vga.h>

#include <sys/time.h>

#include "general.h"
#include "paint.h"
#include "options.h"


void torep (int, int);


struct reps
  {
    int x, y;
    struct reps *nxt;
  } *rb;
  

int him_dirmode = 0;
int him_is_SDL = 0;
static int sx = 0, sy = 0, sc = 0, sl = 0, raw = 0;
static int ucln = 0;
static char **layers;
static char *scrn;
static unsigned int strttim = 0;


#ifdef TANK
extern int opt_col_wtr;
#endif



int
him_init (x, y, c, l, rw)
     int x, y, c, l, rw;
{
  char smode[16];
  int foo, md;

  sx = x;
  sy = y;
  sc = c;
  sl = l;
  
  if (l < 1)
    return -1;
    
  rb = NULL;

  switch (x)
    {
      case 320: case 360: case 640: case 720: case 800: case 1024:
       case 1152: case 1280: case 1600: break;
      default: printf ("him_init: can\'t set horizontal resolution to %d pixels\n", x);
               return -1;
    }

  switch (y)
    {
      case 200: case 240: case 348: case 350: case 400: case 480:
       case 600: case 768: case 864: case 1024: case 1200: break;
      default: printf ("him_init: can\'t set vertical resolution to %d pixels\n", y);
               return -1;
    }
    
  smode[0] = 'G';
  smode[1] = '\0';
  strcat (smode, itoaH (x));
  strcat (smode, "x");
  strcat (smode, itoaH (y));
  strcat (smode, "x");

  switch (c)
    {
      case 2: case 16: case 256: strcat (smode, itoaH (c)); break;
      case 32768: strcat (smode, "32K"); break;
      case 65536: strcat (smode, "64K"); break;
      case 16777216: strcat (smode, "16M"); break;
      default: printf ("him_init: can't set %d colors\n", c);
               return -1;
    }

  if (vga_init ())
    {
      printf ("him_init: vga_init failed\n");
      return -1;
    }
    
  md = vga_getmodenumber (smode);
  if (md == -1)
    {
      printf ("him_init: vga_getmodenumber (%s) failed\n", smode);
      return -1;
    }
  
  layers = (char **) calloc (l, sizeof (char *)); 
  if (layers == NULL)
    {
      printf ("him_init: calloc (%d, sizeof (char *)) failed\n", l);
      return -1;
    }
  
  for (foo = 0; foo < l; foo++)
    {
      int baz;
      *(layers + foo) = (char *) calloc (x * y * ((c == 16 || c == 2) ? 1 : (c / 256)), sizeof (char));

      if ((*(layers + foo)) == NULL)
        {
          printf ("him_init (): calloc (%d) failed\n", (x * y * ((c == 16 || c == 2) ? 1 : (c / 256))));
          sl = foo;
          him_destroy ();
          return -1;
        }
      
      for (baz = 0; baz < (x * y * ((c == 16 || c == 2) ? 1 : (c / 256))); baz++)
        *((*(layers + foo)) + baz) = -1;
    }

  scrn = (char *) calloc (x * y * ((c == 16 || c == 2) ? 1 : (c / 256)), sizeof (char));
  if (scrn == NULL)
    {
      printf ("him_init (): not enough memory\n");
      him_destroy ();
      return -1;
    }

  for (foo = 0; foo < (x * y * ((c == 16 || c == 2) ? 1 : (c / 256))); foo++)
    scrn[foo] = 0;

  if (vga_setmode (md) == -1)
    {
      printf ("him_init: vga_setmode (%d) failed\n", md);
      return -1;
    }

  if (rw)
    {
      raw = 1;
      keyboard_init ();
    }

  strttim = him_getnow();

  printf ("him_init ... [  %c[32mOK%c[0m  ]\n", 27, 27);
  return 0;
}


void
him_clrscr ()
{
  int foo, bar;
  
  if (ucln & 1)
    {
      for (foo = 0; foo < sl; foo++)
        for (bar = 0; bar < (sx * sy * ((sc == 16 || sc == 2) ? 1 : (sc / 256))); bar++)
          *((*(layers + foo)) + bar) = -1;

      for (foo = 0; foo < (sx * sy * ((sc == 16 || sc == 2) ? 1 : (sc / 256))); foo++)
        scrn[foo] = 0;
    }
  if (ucln & 2)
    vga_clear ();

  ucln = 0;
}


void
him_destroy ()
{
  int foo;

  if (sx == 0)
    return;
    
  if (layers != NULL)
    {
      for (foo = 0; foo < sl; foo++)
        if (*(layers + foo) != NULL) 
          free (*(layers + foo));
      free (layers);
    }
  
  if (scrn != NULL)
    free (scrn);
  
  while (rb != NULL)
    {
      struct reps *ra;
      ra = rb;
      rb = rb->nxt;
      free (ra);
    }

  if (raw)
    keyboard_close ();
  else
    him_keykill ();

  sx = sy = sc = sl = raw = 0;
  vga_setmode (TEXT);
}


int
him_pixel (x, y, c, l)
      int x, y, c, l;
{
  int foo;
  char *bar;

  if ((x < 0) || (y < 0) || (x >= sx) || (y >= sy) || (l < -1) || (l > sl) || (c < -1) || (c >= sc))
    return -1;

  if (l == -1)
    return him_putpixel (x, y, c);

  if ((ucln & 1) == 0)
    ucln++;
    
  *((*(layers + l)) + (y * sx) + x) = c;

  if (*(bar = (scrn + (y * sx) + x)) != (foo = him_getpixel (x, y, -1)))
    {
      *bar = foo;
      torep (x, y);
    }
 
  if (him_dirmode)
    him_repaint ();

  if (opt_v && (c == 0) && (l != 7) && (l != 0))
    printf ("him_pixel (): warning: color set to 0 (layer %d)\n", l);
 
  return 0;
}


int
him_putpixel (x, y, c)
    int x, y, c;
{
  if ((x < 0) || (y < 0) || (x >= sx) || (y >= sy) || (c < 0) || (c >= sc))
    return -1;

  if ((ucln & 2) == 0)
    ucln += 2;
    
  vga_setcolor (c);
  vga_drawpixel (x, y);
 
  return 0;
}


int
him_getpixel (x, y, l)
     int x, y, l;
{
  if ((x < 0) || (y < 0) || (x >= sx) || (y >= sy) || (l < -2) || (l > sl))
    return -2;
  
  if (l == -1)
    {
      int foo, baz;
      for (foo = (sl - 1); foo >= 0; foo--)
        {
          baz = *((*(layers + foo)) + (y * sx) + x);
#ifdef TANK
          if ((foo == 1) && (baz == opt_col_wtr) && (rand () % 2) && (*((*(layers + foo)) + (y * sx) + x) > 0))
            baz = *((*(layers + foo)) + (y * sx) + x);   /* these two lines (.-1,.) and the extern int opt_col_wtr make water and clouds 50% transparent in case of sun/moon/star behind */
#endif
          if (baz != -1)
            return baz;
        }
      return 0;
    }
  else if (l == -2)
    {
      int foo, bar = 0;

      for (foo = 0; foo < sl; foo++)
        bar += (((*(*(layers + foo) + (y * sx) + x)) == -1) ? 0 : (pwr (2, foo)));
      
      return bar;
    }
  else
    return *((*(layers + l)) + (y * sx) + x);
}


int
him_uline (x1, y1, x2, y2, c, l, f)
    int x1, y1, x2, y2, c, l;
    int (*f) (int, int, int, int);
{
  int dx, dy, m = 0;

  if ((l < -1) || (l > sl) || (c < -1) || (c >= sc))
    return -1;

  dx = x2 - x1;
  dy = y2 - y1;

  if (dx == 0)
    {
      int n;

      for (n = ((dy < 1) ? -1 : 1); y1 != y2; y1 += n)
        m += ((*f) (x1, y1, c, l));
   
      return m;
    }
      
  if (dy == 0)
    {
      int n;

      for (n = ((dx < 1) ? -1 : 1); x1 != x2; x1 += n)
        m += ((*f) (x1, y1, c, l));

      return m;
    }
      
  if (abs(dx) == abs(dy))
    {
      int sy = dy / abs (dy);
      int sx = dx / abs (dx);
      
      for (dx = 0; dx <= abs (dy); dx++)
        m += ((*f) (x1 + (dx * sx), y1 + (dx * sy), c, l));

      return m;
    }

  if (abs(dx) > abs(dy))
    {
      int xx; 
      int sy = dy / abs (dy);
      int sx = dx / abs (dx);

      for (xx = 0; xx <= abs (dx); xx++)
        m += ((*f) (x1 + (xx * sx), y1 + roundH (sy * xx * ((float) abs (dy) / (float) abs (dx))), c, l));

      return m;
    }

  if (abs(dy) > abs(dx))
    {
      int yy; 
      int sy = dy / abs (dy);
      int sx = dx / abs (dx);

      for (yy = 0; yy <= abs (dy); yy++)
        m += ((*f) (x1 + roundH (sx * yy * ((float) abs (dx) / (float) abs (dy))), y1 + (yy * sy), c, l));
      return m;
    }
  
  return -2;
}


int
him_line (x1, y1, x2, y2, c, l)
    int x1, y1, x2, y2, c, l;
{
  return him_uline (x1, y1, x2, y2, c, l, &him_pixel);
}


int
him_box (x1, y1, x2, y2, c, l)
  int x1, y1, x2, y2, c, l;
{
  int nx, ny, y;

  if ((x1 < 0) || (y1 < 0) || (x1 >= sx) || (y1 >= sy) || (x2 < 0) || (y2 < 0) || (x2 >= sx) || (y2 >= sy) || (l < -1) || (l > sl) || (c < -1) || (c >= sc))
    return -1;

  for (nx = ((x1 < x2) ? 1 : -1); x1 != (x2 + nx); x1 += nx)
    for (y = y1, ny = ((y1 < y2) ? 1 : -1); y != (y2 + ny); y += ny)
      if (him_pixel (x1, y, c, l) == -1)
        return -1;

  return 0;
}


int
him_circle (x, y, r, c, l)
    int x, y, r, c, l;
{
  int xx, n = 0;

  if ((r < 0) || (l < -1) || (l > sl) || (c < -1) || (c >= sc))
    return -1;

  for (xx = (r / 4); xx < r; xx++)
    {
      float yy;
      
      yy = sqrt ((r * r) - ((r - xx) * (r - xx)));
      
      n += him_pixel (x - r + xx, (int) (y - yy), c, l);
      n += him_pixel (x - r + xx, (int) (y + yy), c, l);
      n += him_pixel (x + r - xx, (int) (y - yy), c, l);
      n += him_pixel (x + r - xx, (int) (y + yy), c, l);

      n += him_pixel ((int) (x - yy), y - r + xx, c, l);
      n += him_pixel ((int) (x + yy), y - r + xx, c, l);
      n += him_pixel ((int) (x - yy), y + r - xx, c, l);
      n += him_pixel ((int) (x + yy), y + r - xx, c, l);
      
      if (n < 0)
        return -1;
    }

  n += him_pixel (x + r, y, c, l);
  n += him_pixel (x - r, y, c, l);
  n += him_pixel (x, y + r, c, l);
  n += him_pixel (x, y - r, c, l);
  
  if (n < 0)
    return -1;

  return 0;
}


int
him_ufullcircle (x, y, r, c, l, f)
    int x, y, r, c, l;
    int (*f) (int, int, int, int);
{
  int xx, n = 0;
  
  if ((r < 0) || (l < -1) || (l > sl) || (c < -1) || (c >= sc))
    return -1;

  for (xx = (r / 4); xx < r; xx++)
    {
      float yy;
      yy = sqrt ((r * r) - ((r - xx) * (r - xx)));
      
      n += him_uline (x - r + xx, (int) (y - yy), x - r + xx, (int) (y + yy), c, l, f);
      n += him_uline (x + r - xx, (int) (y - yy), x + r - xx, (int) (y + yy), c, l, f);

      n += him_uline ((int) (x - yy), y - r + xx, (int) (x - yy), y + r - xx, c, l, f);
      n += him_uline ((int) (x + yy), y - r + xx, (int) (x + yy), y + r - xx, c, l, f);
    }

  n += him_uline (x - r, y, x + r, y, c, l, f);
  n += him_uline (x, y - r, x, y + r, c, l, f);

  if (n < 0)
    return -1;

  return 0;
}


int
him_ufullcircler (x, y, r1, r2, c, l, f)
    int x, y, r1, r2, c, l;
    int (*f) (int, int, int, int);
{
  int xx, n = 0;
  
  if ((r1 < 0) || (r2 < 0) || (l < -1) || (l > sl) || (c < -1) || (c >= sc))
    return -1;

  if (r1 > r2)
    {
      n = r1;
      r1 = r2;
      r2 = n;
      n = 0;
    }

  for (xx = 0; xx < r2; xx++)
    {
      int y1, y2;
      y2 = (int) sqrt ((r2 * r2) - (xx * xx));
      y1 = ((xx > r1) ? (0) : ((int) sqrt ((r1 * r1) - (xx * xx))));
      
      n += him_uline (x - xx, y - y1, x - xx, y - y2, c, l, f);
      n += him_uline (x - xx, y + y1, x - xx, y + y2, c, l, f);
    
      n += him_uline (x + xx, y - y1, x + xx, y - y2, c, l, f);
      n += him_uline (x + xx, y + y1, x + xx, y + y2, c, l, f);
    }

  n += him_uline (x - r1, y, x - r2, y, c, l, f);
  n += him_uline (x + r1, y, x + r2, y, c, l, f);
  n += him_uline (x, y - r1, x, y - r2, c, l, f);
  n += him_uline (x, y + r1, x, y + r2, c, l, f);

  if (n < 0)
    return -1;

  return 0;
}


int
him_filledtriangle (x1, y1, x2, y2, x3, y3, c, l)
   int x1, y1, x2, y2, x3, y3, c, l;
{
  int x0, y0, n = 0;
  
  x0 = (x1 + x2 + x3) / 3;
  y0 = (y1 + y2 + y3) / 3;
  
  while ((near (x1, y1, x0, y0) > 1) || (near (x2, y2, x0, y0) > 1) || (near (x3, y3, x0, y0) > 1))
    {
      n += him_line (x1, y1, x2, y2, c, l);
      n += him_line (x1, y1, x3, y3, c, l);
      n += him_line (x3, y3, x2, y2, c, l);
      
      if (x0 > x1)
        x1++;
      if (x0 < x1)
        x1--;
      if (y0 > y1)
        y1++;
      if (y0 < y1)
        y1--;

      if (x0 > x2)
        x2++;
      if (x0 < x2)
        x2--;
      if (y0 > y2)
        y2++;
      if (y0 < y2)
        y2--;

      if (x0 > x3)
        x3++;
      if (x0 < x3)
        x3--;
      if (y0 > y3)
        y3++;
      if (y0 < y3)
        y3--;
    }

  return n;
}


void
him_repaint ()
{
  static int curc = ~0;

  while (rb != NULL)
    {
      struct reps *ra;
      ra = rb;
      rb = rb->nxt;

      if (curc != *(scrn + (ra->y * sx) + ra->x))
        vga_setcolor (curc = *(scrn + (ra->y * sx) + ra->x));
      vga_drawpixel (ra->x, ra->y);
      
      free (ra);
    }
}


void
him_keykill ()
{
  while (him_getkey ());
}


int
him_getkey ()
{
  return vga_getkey ();
}


void
him_setpalette (c, r, g, b)
    int c, r, g, b;
{
  if ((c < 0) || (c >= sc))
    return;
    
  vga_setpalette (c, r, g, b);
}


void
him_getpalette (c, r, g, b)
    int c, *r, *g, *b;
{
  if ((c < 0) || (c >= sc))
    return;
    
  vga_getpalette (c, r, g, b);
}


void
him_getpalvec (s, n, b)
    int s, n, *b;
{
  if ((s < 0) || ((s + n) >= sc))
    return;
    
  vga_getpalvec (s, n, b);
}


void
him_setpalvec (s, n, b)
    int s, n, *b;
{
  if ((s < 0) || ((s + n) >= sc))
    return;
    
  vga_setpalvec (s, n, b);
}


int
him_keyupd ()
{
  return keyboard_update ();
}


int
him_keypr (key)
    int key;
{
  return keyboard_keypressed (key);
}



void
torep (x, y)
    int x, y;
{
  struct reps *ra;
  ra = (struct reps *) calloc (1, sizeof (struct reps));
  if (ra == NULL)
    {
      printf ("(paint) torep: can\'t callocate 1 sizeof (struct reps)\n");
      return;
    }
  ra->x = x;
  ra->y = y;
  ra->nxt = rb;
  rb = ra;
}


int
him_showimg(char *fn)
{
	(void) fn;
	return -1; /* only in paintSDL */
}


void
him_putlock (void)
{
  /* only in paintSDL */
}

void
him_putulock (void)
{
  /* only in paintSDL */
}

void
him_putupd (void)
{
  /* only in paintSDL */
}


unsigned int
him_getnow(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
