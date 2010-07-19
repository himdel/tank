/*
  paintSDL.c - SDL him_painting routines for tank
  himdel@seznam.cz
  
  note: I'm not actually using any SDL abilites yet except pixel
        painting in him_repaint (), init and quit in him_init and
        him_destroy and keyboard thingies :)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "general.h"
#include "paint.h"
#include "SDL.h"
#include "SDL_image.h"


void torep (int, int);
int h__sk2SK (SDLKey);
Uint8 getpixel (SDL_Surface *, int, int);
void putpixel (SDL_Surface *, int, int, Uint32);


struct reps
  {
    int x, y;
    struct reps *nxt;
  } *rb;
  

int him_dirmode = 0;
int him_is_SDL = 1;
int sx = 0, sy = 0, sc = 0, sl = 0, raw = 0;
SDL_Surface **layers;
SDL_Surface *scr;
int keyz[128];
int ucln = 0;

#ifdef TANK
extern int opt_col_wtr;  /* for use in getpixel */
#endif


int
him_init (x, y, c, l, rw)
     int x, y, c, l, rw;
{
  int foo;

  for (foo = 0; foo < 128; foo++)
    keyz[foo] = 0;

  if (SDL_Init (SDL_INIT_VIDEO) == -1)
    {
      printf ("SDL_Init: %s\n", SDL_GetError ());
      return (-1);
    }

#ifdef TANK
  SDL_WM_SetCaption ("tank", "tank");
  SDL_WM_SetIcon (SDL_LoadBMP ("img/icon.bmp"), NULL);
#endif /* TANK */

  scr = SDL_SetVideoMode (x, y, 8, SDL_SWSURFACE);
  if (scr == NULL)
    {
      printf ("SDL_SetVideoMode: %s\n", SDL_GetError ());
      SDL_Quit ();
      return (-1);
    } 

  sx = x;
  sy = y;
  sc = c;
  sl = l;
  raw = rw;
  
  if (l < 1)
    return -1;
    
  rb = NULL;

  layers = (SDL_Surface **) calloc (l, sizeof (SDL_Surface *)); 
  if (layers == NULL)
    {
      printf ("him_init: calloc (%d, sizeof (SDL_Surface *)) failed\n", l);
      return -1;
    }
  
  for (foo = 0; foo < l; foo++)
    {
      int x, y;
      
      *(layers + foo) = SDL_CreateRGBSurface (SDL_SWSURFACE, 640, 480, 8, 0xff, 0xff00, 0xff0000, 0xff000000);

      if ((*(layers + foo)) == NULL)
        {
          printf ("him_init (): SDL_CreateRGBSurface failed\n");
          sl = foo;
          him_destroy ();
          return -1;
        }
      
      if (SDL_MUSTLOCK ((*(layers + foo))))
        SDL_LockSurface (*(layers + foo));

      for (x = 0; x < 640; x++)
        for (y = 0; y < 480; y++)
          putpixel (*(layers + foo), x, y, 0xff);

      if (SDL_MUSTLOCK ((*(layers + foo))))
        SDL_UnlockSurface (*(layers + foo));
    }

  if (c >= 16)  /* set standard VGA 0-15 colors */
    {
      SDL_Color colors[16];

      colors[0].r = 0;
      colors[0].g = 0;
      colors[0].b = 0;

      colors[1].r = 0;
      colors[1].g = 0;
      colors[1].b = 168;

      colors[2].r = 0;
      colors[2].g = 168;
      colors[2].b = 0;

      colors[3].r = 0;
      colors[3].g = 168;
      colors[3].b = 168;

      colors[4].r = 168;
      colors[4].g = 0;
      colors[4].b = 0;

      colors[5].r = 168;
      colors[5].g = 0;
      colors[5].b = 168;

      colors[6].r = 168;
      colors[6].g = 84;
      colors[6].b = 0;

      colors[7].r = 168;
      colors[7].g = 168;
      colors[7].b = 168;

      colors[8].r = 84;
      colors[8].g = 84;
      colors[8].b = 84;

      colors[9].r = 84;
      colors[9].g = 84;
      colors[9].b = 252;

      colors[10].r = 84;
      colors[10].g = 252;
      colors[10].b = 84;
      
      colors[11].r = 84;
      colors[11].g = 252;
      colors[11].b = 252;
      
      colors[12].r = 252;
      colors[12].g = 84;
      colors[12].b = 84;
      
      colors[13].r = 252;
      colors[13].g = 84;
      colors[13].b = 252;
      
      colors[14].r = 252;
      colors[14].g = 252;
      colors[14].b = 84;
      
      colors[15].r = 252;
      colors[15].g = 252;
      colors[15].b = 252;

      SDL_SetColors (scr, colors, 0, 16);
    }

  printf ("him_init ... [  %c[32mOK%c[0m  ]\n", 27, 27);
  printf ("\nUsing libraries SDL and SDL_image, licensed under LGPL and\n");
  printf ("downloadable from http://www.libsdl.org/.\n\n");

#ifdef TANK
  him_showimg ("img/intro.png");
  sleep (1);
#endif /* TANK */  

  return 0;
}


void
him_clrscr ()
{
  int foo, x, y;
  
  if (ucln & 1)
    {
      for (foo = 0; foo < sl; foo++)
        {
          if (SDL_MUSTLOCK ((*(layers + foo))))
            SDL_LockSurface (*(layers + foo));

          for (x = 0; x < 640; x++)
            for (y = 0; y < 480; y++)
              putpixel (*(layers + foo), x, y, 0xff);

          if (SDL_MUSTLOCK ((*(layers + foo))))
            SDL_UnlockSurface (*(layers + foo));
        }
    }

  if (ucln & 2)
    {
      if (SDL_MUSTLOCK (scr))
        SDL_LockSurface (scr);

      for (x = 0; x < 640; x++)
        for (y = 0; y < 480; y++)
          putpixel (scr, x, y, 0);

      if (SDL_MUSTLOCK (scr))
        SDL_UnlockSurface (scr);

      if (sc >= 16)  /* set standard VGA 0-15 colors */
        {
          SDL_Color colors[16];

          colors[0].r = 0;
          colors[0].g = 0;
          colors[0].b = 0;

          colors[1].r = 0;
          colors[1].g = 0;
          colors[1].b = 168;

          colors[2].r = 0;
          colors[2].g = 168;
          colors[2].b = 0;

          colors[3].r = 0;
          colors[3].g = 168;
          colors[3].b = 168;
    
          colors[4].r = 168;
          colors[4].g = 0;
          colors[4].b = 0;
    
          colors[5].r = 168;
          colors[5].g = 0;
          colors[5].b = 168;

          colors[6].r = 168;
          colors[6].g = 84;
          colors[6].b = 0;

          colors[7].r = 168;
          colors[7].g = 168;
          colors[7].b = 168;

          colors[8].r = 84;
          colors[8].g = 84;
          colors[8].b = 84;

          colors[9].r = 84;
          colors[9].g = 84;
          colors[9].b = 252;

          colors[10].r = 84;
          colors[10].g = 252;
          colors[10].b = 84;
      
          colors[11].r = 84;
          colors[11].g = 252;
          colors[11].b = 252;
      
          colors[12].r = 252;
          colors[12].g = 84;
          colors[12].b = 84;
      
          colors[13].r = 252;
          colors[13].g = 84;
          colors[13].b = 252;
      
          colors[14].r = 252;
          colors[14].g = 252;
          colors[14].b = 84;
      
          colors[15].r = 252;
          colors[15].g = 252;
          colors[15].b = 252;

          SDL_SetColors (scr, colors, 0, 16);
        }
    }

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
          SDL_FreeSurface (*(layers + foo));
      free (layers);
    }
  
  while (rb != NULL)
    {
      struct reps *ra;
      ra = rb;
      rb = rb->nxt;
      free (ra);
    }

  sx = sy = sl = sc = raw = 0;
  SDL_Quit ();
}


int
him_pixel (x, y, c, l)
      int x, y, c, l;
{
  if ((x < 0) || (y < 0) || (x >= sx) || (y >= sy) || (l < -1) || (l > sl) || (c < -1) || (c >= sc))
    return -1;

  if (l == -1)
    return him_putpixel (x, y, c);

  if ((ucln & 1) == 0)
    ucln ++;

  if (SDL_MUSTLOCK ((*(layers + l))))
    SDL_LockSurface (*(layers + l));

  if (c == -1)
    c = 0xff;
    
  putpixel (*(layers + l), x, y, c);

  if (SDL_MUSTLOCK ((*(layers + l))))
    SDL_UnlockSurface (*(layers + l));

  torep (x, y);
 
  if (him_dirmode)
    him_repaint ();
 
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

  if (SDL_MUSTLOCK (scr))
    SDL_LockSurface (scr);

  putpixel (scr, x, y, c);

  if (SDL_MUSTLOCK (scr))
    SDL_UnlockSurface (scr);
 
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
      int foo;
      Uint8 bar = 0, baz;
      
      for (foo = 0; foo < sl; foo++)
        {
          SDL_UpdateRect (layers[foo], 0, 0, 0, 0);
          
          if (SDL_MUSTLOCK ((*(layers + foo))))
            SDL_LockSurface (*(layers + foo));
 
          baz = getpixel (*(layers + foo), x, y);
 
          if (SDL_MUSTLOCK ((*(layers + foo))))
            SDL_UnlockSurface (*(layers + foo));

#ifdef TANK
          if (bar && (((foo == 1) && (baz == opt_col_wtr)) || (foo == 2)) && (rand () % 2))
            baz = bar;   /* these two lines (.-1,.) and the extern int opt_col_wtr make water and clouds 50% transparent in case of sun/moon/star behind */
#endif

          if (baz != 0xff)
            bar = baz;
        }
      return bar;
    }
  else if (l == -2)
    {
      int foo;
      Uint8 bar = 0;

      for (foo = 0; foo < sl; foo++)
        {
          SDL_UpdateRect (layers[foo], 0, 0, 0, 0);
          
          if (SDL_MUSTLOCK ((*(layers + foo))))
            SDL_LockSurface (*(layers + foo));

          bar += ((getpixel (*(layers + foo), x, y) == 0xff) ? 0 : (pwr (2, foo)));

          if (SDL_MUSTLOCK ((*(layers + foo))))
            SDL_UnlockSurface (*(layers + foo));
        }

      return (int) bar;
    }
  else
    {
      Uint8 foo;
      
      SDL_UpdateRect (layers[l], 0, 0, 0, 0);
          
      foo = getpixel (*(layers + l), x, y);

      if (SDL_MUSTLOCK ((*(layers + l))))
        SDL_LockSurface (*(layers + l));

      if (SDL_MUSTLOCK ((*(layers + l))))
        SDL_UnlockSurface (*(layers + l));

      if (foo == 0xff)
        return -1;

      return (int) foo;
    }
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
  if (rb == NULL)
    return;

  if (SDL_MUSTLOCK (scr) && (SDL_LockSurface (scr) < 0))
    return;
          
  while (rb != NULL)
    {
      struct reps *ra;
      ra = rb;
      rb = rb->nxt;
      
      putpixel (scr, ra->x, ra->y, him_getpixel (ra->x, ra->y, -1));

      free (ra);
    }

  if (SDL_MUSTLOCK (scr))
    SDL_UnlockSurface (scr);

  SDL_UpdateRect (scr, 0, 0, 0, 0);
}


void
him_keykill ()
{
/* NOT NEEDED */
}


int
him_getkey ()
{
  SDL_Event ev;
  
  while (SDL_PollEvent (&ev))
    {
      switch (ev.type)
        {
          case SDL_KEYDOWN:
            return (ev.key.keysym.sym);  /* usually works but not well - Shift+a doesnt return 'A' but ?, 'a' */
            break;
          case SDL_KEYUP:
            break;
          case SDL_QUIT:  /* just returns escape - clumsy but works */
            return 27;
        }
    }

  return 0;
}


void
him_setpalette (c, r, g, b)
    int c, r, g, b;
{
  SDL_Color color;

  if ((c < 0) || (c >= sc) || (r < 0) || (g < 0) || (b < 0) || (r > 255) || (g > 255) || (b > 255))
    return;

  color.r = r;
  color.g = g;
  color.b = b;

  SDL_SetColors (scr, &color, c, 1);
}


void
him_getpalette (c, r, g, b)
    int c, *r, *g, *b;
{
  if ((c < 0) || (c >= sc))
    return;

  *r = scr->format->palette->colors[c].r;
  *g = scr->format->palette->colors[c].g;
  *b = scr->format->palette->colors[c].b;
}


void
him_getpalvec (s, n, b)
    int s, n, *b;
{
  int foo;

  if ((s < 0) || (s >= sc) || (n < 0) || ((s + n) >= sc) || (b == NULL))
    return;
    
  for (foo = 0; foo < n; foo++)
    {
      *(b + (3 * foo)) = scr->format->palette->colors[s + foo].r;
      *(b + (3 * foo) + 1) = scr->format->palette->colors[s + foo].g;
      *(b + (3 * foo) + 2) = scr->format->palette->colors[s + foo].b;
    }
}


void
him_setpalvec (s, n, b)
    int s, n, *b;
{
  int foo;
  SDL_Color *col;

  if ((s < 0) || (s >= sc) || (n < 0) || ((s + n) >= sc) || (b == NULL))
    return;
  
  col = (SDL_Color *) calloc (n, sizeof (SDL_Color));
  
  for (foo = 0; foo < n; foo++)
    {
      col[foo].r = *(b + (3 * foo));
      col[foo].g = *(b + (3 * foo) + 1);
      col[foo].b = *(b + (3 * foo) + 2);
    }
  
  SDL_SetColors (scr, col, s, n);
  
  free (col);
}


/* TODO implement all keys, not just the ones I use in tank */
int
h__sk2SK (key)
   SDLKey key;
{
  switch (key)
    {
      case SDLK_q: 
        return SCANCODE_Q;
      case SDLK_w:
        return SCANCODE_W;
      case SDLK_e:
        return SCANCODE_E;
      case SDLK_r:
        return SCANCODE_R;
      case SDLK_a:
        return SCANCODE_A;
      case SDLK_s:
        return SCANCODE_S;
      case SDLK_d:
        return SCANCODE_D;
      case SDLK_f:
        return SCANCODE_F;
      case SDLK_z:
        return SCANCODE_Z;
      case SDLK_x:
        return SCANCODE_X;
      case SDLK_c:
        return SCANCODE_C;
      case SDLK_v:
        return SCANCODE_V;
      case SDLK_u:
        return SCANCODE_U;
      case SDLK_i:
        return SCANCODE_I;
      case SDLK_o:
        return SCANCODE_O;
      case SDLK_p:
        return SCANCODE_P;
      case SDLK_h:
        return SCANCODE_H;
      case SDLK_j:
        return SCANCODE_J;
      case SDLK_k:
        return SCANCODE_K;
      case SDLK_l:
        return SCANCODE_L;
      case SDLK_b:
        return SCANCODE_B;
      case SDLK_n:
        return SCANCODE_N;
      case SDLK_m:
        return SCANCODE_M;
      case SDLK_COMMA:
        return SCANCODE_COMMA;
      case SDLK_RETURN:
        return SCANCODE_ENTER;
      case SDLK_SPACE:
        return SCANCODE_SPACE;
      case SDLK_ESCAPE:
        return SCANCODE_ESCAPE;
      case SDLK_UP:
        return SCANCODE_CURSORBLOCKUP;
      case SDLK_DOWN:
        return SCANCODE_CURSORBLOCKDOWN;
      case SDLK_LEFT:
        return SCANCODE_CURSORBLOCKLEFT;
      case SDLK_RIGHT:
        return SCANCODE_CURSORBLOCKRIGHT;
      case SDLK_BACKQUOTE:
        return SCANCODE_GRAVE;
      default:
        return 0;
    }
}


int
him_keyupd ()
{
  int waz = 0;
  SDL_Event ev;
  
  while (SDL_PollEvent (&ev))
    {
      int foo;
      switch (ev.type)
        {
          case SDL_KEYDOWN:
            foo = h__sk2SK (ev.key.keysym.sym);
            if (foo)
              {
                keyz[foo] = 1;
                waz++;
              }
            break;
          case SDL_KEYUP:
            foo = h__sk2SK (ev.key.keysym.sym);
            if (foo)
              {
                keyz[foo] = 0;
                waz++;
              }
            break;
          case SDL_QUIT:  /* just sets escape - clumsy but works */
            waz++;
            keyz[1] = 1;
            break;
        }
    }
  
  return waz;
}


int
him_keypr (key)
    int key;
{
  return keyz[key];
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


Uint8
getpixel (surface, x, y)
    SDL_Surface *surface;
    int x, y;
{
  return *((Uint8 *) surface->pixels + (surface->pitch * y) + x);
}


void
putpixel (surface, x, y, pixel)
    SDL_Surface *surface;
    int x, y;
    Uint32 pixel;
{
  *((Uint8 *)surface->pixels + (surface->pitch * y) + x) = pixel;
}


int
him_showimg (fn)
    char *fn;
{
  SDL_Surface *img;
  
  img = IMG_Load (fn);
  if (img == NULL)
    return -1;
  
  if (img->format->palette && scr->format->palette)
    SDL_SetColors (scr, img->format->palette->colors, 0, img->format->palette->ncolors);
  
  SDL_BlitSurface (img, NULL, scr, NULL);
  SDL_UpdateRect (scr, 0, 0, 0, 0);
  SDL_FreeSurface (img);
  
  return 0;
}


void
him_putlock (void)
{
  if (SDL_MUSTLOCK (scr))
    SDL_LockSurface (scr);
}


void
him_putulock (void)
{
  if (SDL_MUSTLOCK (scr))
    SDL_UnlockSurface (scr);
}


void
him_putupd (void)
{
  SDL_UpdateRect (scr, 0, 0, 0, 0);
}


unsigned int
him_getnow(void)
{
	return SDL_GetTicks();
}
