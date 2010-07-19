#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "paint.h"
#include "letters.h"
#include "options.h"
#include "general.h"
#include "water.h"


#ifndef PI
# ifdef M_PI
#  define PI M_PI
# else
#  define PI 3.14
# endif
#endif


#define SCR_X 640
#define SCR_Y 480
#define SCR_C 256


struct playa
  {
    int x, a, p, l, e;		/* tank X coord, angle, power, lives, weap energy */
  };

struct spos
  {
    int x, y, a;
  } *c;       /* clouds */

struct shotp
  {
    float x, y;
    struct shotp *nxt;
  };

struct shootp
  {
    struct shotp *shob, *shol;
    float vx, vy;
    int shon;
    int hit;
    struct shootp *nxt;
    int who;
  } *shoo;

struct expl
  {
    int x, y, p;
    float c, d;
    struct expl *nxt, *pre;
  } *expb;

struct lght
  {
    int x, y;
    int h, nc;  /* hit, numchildren */
    struct lght *n, *p, *u, **c;  /* next, prev, up, children */
  };

struct lghtt
  {
    int w, h;  /* who, hit */
    int numforks;
    struct lght *lb, *ls, *lp;  /* first, first-to-process, last */
    struct lghtt *p, *n;
  } *lghtb;

struct lndpts
  {
    int x, y;
    struct lndpts *n, *p;
    unsigned int tm;
  } *lndpt;



int lmhs[SCR_X];    /* land max heights */
int nc;  /* num clouds */


void paint_stars (int);
void gen_paint_land (void);	/* std bezier land */
void gen_paint_clouds (int);
void bonuses (void);
void paint_tanx (int, int, int, int);
void show_arrows (int, int);
void showscore (struct playa *);
void checkkeys (struct playa *);
struct shootp *fire (struct playa *, int);
void do_we (struct playa *);
void do_fire (struct playa *);
void do_expl (struct playa *);
void do_light (struct playa *);
void do_land (void);

struct shotp *frees (struct shotp *, int *);
struct shootp *freeso (struct shootp *);
void addshpx (struct shootp *, double, double);
void addexplpnt (int, int, int);
void newlight (int, int, int);
void light_depaint (struct lght *);
void addlndpt (int, int);

void cloud (struct spos *);
int bezier (int, int, int *, int *, int);
int gplnd (int, int, int, int);
int px_hole (int, int, int, int);
int px_cloud (int, int, int, int);
int px_moon (int, int, int, int);

int lndrnd (int);

void showebar (int, int);


struct Tasks {
	unsigned int ms;
	void (*func) (struct playa *);
	unsigned int last;		/* init to 0 please */
} tsk[] = {
	{1000, do_we, 0},
	{10, do_fire, 0},
	{20, do_expl, 0},
	{100, do_light, 0},
	{62, (void (*) (struct playa *)) do_land, 0},
	{500, (void (*) (struct playa *)) rewater, 0},
	{0, NULL, 0}
};



int
main (void)
{
	struct playa p[2];

	printf ("tank\nMartin HRADIL\nhttp://github.com/himdel/tank\n\n");
	opt_init ();  /* set or load options */

	if (him_init (SCR_X, SCR_Y, SCR_C, 8, 1) == -1)   /* initialize graphics || die */
	exit (1);

	atexit (him_destroy);

	letters_init (NULL);  /* initialize text-writing routines */

	srand (time (NULL));

	him_clrscr ();
	water_init ();

	nc = (rand () % 6) + 2;
	c = (struct spos *) calloc (nc, sizeof (struct spos));

	expb = (struct expl *) NULL;
	shoo = (struct shootp *) NULL;
	lndpt = (struct lndpts *) NULL;

	wrtwrd (247, 232, "GENERATING TERRAIN", 12, 0, 7);
	him_repaint ();

	paint_stars(opt_num_stars);   /* paints stars */
	gen_paint_land();   /* generates and paints land */
	water_land(lmhs);   /* fill valleys with water */
	gen_paint_clouds(nc);  /* generates and paints clouds */

	p[0].x = (rand() % 288) + 16;       /* generates positions */
	p[1].x = (rand() % 288) + 336;
	p[0].a = 32; p[1].a = 96;    /* sets angles, forces and lives */
	p[0].p = p[1].p = 127;
	p[0].l = p[1].l = opt_ilives;
	p[0].e = p[1].e = opt_iwe;

	paint_tanx (p[0].x, p[0].a, p[1].x, p[1].a);   /* paints tanks */

	show_arrows (p[0].x, p[1].x);
	showscore (p);

	wrtwrd (247, 232, "GENERATING TERRAIN", -1, -1, 7);
	him_repaint ();

	while ((p[0].l > 0) && (p[1].l > 0)) {   /* main loop */
		unsigned int now = him_getnow();
		struct Tasks *t = tsk;

		while ((t != NULL) && (t->ms != 0)) {
			if (t->last + t->ms <= now) {
				t->func(p);
				t->last = now;
			}
			t++;
		}

		checkkeys (p);
		showebar (p[0].e, p[1].e);
		him_repaint ();
	}

	if ((p[0].l <= 0) && (p[1].l <= 0))
		printf ("\ngame quit\n");
	else if (p[0].l <= 0)
		printf ("\nplayer 1 wins!\n");
	else
		printf ("\nplayer 0 wins!\n");

	water_destroy ();
	while ((shoo = freeso (shoo)) != NULL)
		;
	while (expb != NULL) {
		struct expl *ex;
		ex = expb;
		expb = expb->nxt;
		free (ex);
	}
	free (c);

	letters_destroy ();   /* free font */
	him_destroy ();       /* free graphics */

	exit (0);   /* die */
}



void
paint_stars (n)
    int n;
{
  int foo;
  if ((rand () % 3) > 1)
    {
      int x, y;
      foo = (rand () % 97) - 48;  /* phase (-48..48) */
      him_ufullcircle (x = (rand () % SCR_X), y = (rand () % 240), 24, opt_col_moon, 0, &px_moon);
      him_ufullcircle (x - (24 * ((foo < 1) ? 1 : -1)), y, abs (foo), 0, 0, &him_pixel);
    }

  for (foo = 0; foo < n; foo++)
    him_pixel (rand () % SCR_X, rand () % SCR_Y, opt_col_star, 0);
  
  if ((rand () % 3) > 1)
    him_ufullcircle (rand () % SCR_X, rand () % 240, 32, opt_col_sun, 0, &him_pixel);
}


void
gen_paint_land ()	/* std bezier land */
{
  int foo;
  int *px, *py;

  px = (int *) malloc (opt_lndpts * sizeof (int));
  py = (int *) malloc (opt_lndpts * sizeof (int));
 
  if ((px == NULL) || (py == NULL))
    {
      free (c);
      letters_destroy ();   /* free font */
      him_destroy ();       /* free graphics */
      exit (1);
    }

  for (foo = 0; foo < opt_lndpts; foo++)
    {
      px[foo] = (rand () % 800) - 80;
      py[foo] = (rand () % 600) - 60;
    }

  for (foo = 0; foo < SCR_X; foo += opt_jmp)
    {
      lmhs[foo] = bezier (foo, opt_lndpts, px, py, SCR_X - 1);

      if (lmhs[foo] < 0)
        lmhs[foo] = 0;
      if (lmhs[foo] >= SCR_Y)
        lmhs[foo] = SCR_Y - 1; 
    
      if (foo)
        him_uline (foo - opt_jmp, lmhs[foo - opt_jmp], foo, lmhs[foo], opt_col_land, 1, &gplnd);
    }

  lmhs[SCR_X - 1] = bezier (SCR_X - 1, opt_lndpts, px, py, SCR_X - 1);

  if (lmhs[SCR_X - 1] < 0)
    lmhs[SCR_X - 1] = 0;
  if (lmhs[SCR_X - 1] > SCR_Y - 1)
    lmhs[SCR_X - 1] = SCR_Y - 1; 
    
  him_uline (foo - opt_jmp, lmhs[foo - opt_jmp], SCR_X - 1, lmhs[SCR_X - 1], opt_col_land, 1, &gplnd);
  
  free (px);
  free (py);
}


void
gen_paint_clouds (n)
    int n;
{
  int foo;
  
  for (foo = 0; foo < n; foo++)
    {
      cloud (c + foo);
      if (opt_v)
        printf ("cloud (%d, %d)\n", c[foo].x, c[foo].y);
    }
}


void
paint_tanx (x1, a1, x2, a2)
     int x1, a1, x2, a2;
{
  static int ox1 = ~0, oa1 = ~0, ox2 = ~0, oa2 = ~0, oy1, oy2;
  int y1, y2;

  y1 = (getwl (x1) ? min(lmhs[x1] - 8, getwl (x1)) : (lmhs[x1] - 8));
  y2 = (getwl (x2) ? min(lmhs[x2] - 8, getwl (x2)) : (lmhs[x2] - 8));

  if ((ox1 != ~0) && ((ox1 != x1) || (oy1 != y1)))
    him_box (ox1 - 32, oy1 - 32, ox1 + 32, oy1 + 32, -1, 5);

  if ((ox2 != ~0) && ((ox2 != x2) || (oy2 != y2)))
    him_box (ox2 - 32, oy2 - 32, ox2 + 32, oy2 + 32, -1, 5);

  if (oa1 != a1)
    {
      him_line (ox1 + (cos (oa1 * PI / 128) * 8), oy1 - (sin (oa1 * PI / 128) * 8), ox1 + (cos (oa1 * PI / 128) * 15), oy1 - (sin (oa1 * PI / 128) * 15), -1, 5);
      him_line (x1 + (cos (a1 * PI / 128) * 8), y1 - (sin (a1 * PI / 128) * 8), x1 + (cos (a1 * PI / 128) * 15), y1 - (sin (a1 * PI / 128) * 15), opt_col_p1, 5);
    }

  if (oa2 != a2)
    {
      him_line (ox2 + (cos (oa2 * PI / 128) * 8), oy2 - (sin (oa2 * PI / 128) * 8), ox2 + (cos (oa2 * PI / 128) * 15), oy2 - (sin (oa2 * PI / 128) * 15), -1, 5);
      him_line (x2 + (cos (a2 * PI / 128) * 8), y2 - (sin (a2 * PI / 128) * 8), x2 + (cos (a2 * PI / 128) * 15), y2 - (sin (a2 * PI / 128) * 15), opt_col_p2, 5);
    }

  if ((ox1 != x1) || (oy1 != y1))
    {
      him_ufullcircle (x1, y1, 8, -1, 1, &px_hole);
      him_ufullcircle (x1, y1, 8, opt_col_p1, 5, &him_pixel);
      him_line (x1 + (cos (a1 * PI / 128) * 8), y1 - (sin (a1 * PI / 128) * 8), x1 + (cos (a1 * PI / 128) * 16), y1 - (sin (a1 * PI / 128) * 16), opt_col_p1, 5);
    }

  if ((ox2 != x2) || (oy2 != y2))
    {
      him_ufullcircle (x2, y2, 8, -1, 1, &px_hole);
      him_ufullcircle (x2, y2, 8, opt_col_p2, 5, &him_pixel);
      him_line (x2 + (cos (a2 * PI / 128) * 8), y2 - (sin (a2 * PI / 128) * 8), x2 + (cos (a2 * PI / 128) * 16), y2 - (sin (a2 * PI / 128) * 16), opt_col_p2, 5);
    }

  ox1 = x1; oy1 = y1; oa1 = a1;
  ox2 = x2; oy2 = y2; oa2 = a2;
}


void
show_arrows (x1, x2)
   int x1, x2;
{
  static int ox1 = ~0, ox2 = ~0;

  x1 -= 8;
  x2 -= 8;

  if ((ox1 != ~0) && (ox1 != x1))
    wrtltrzoom (ox1, 0, 0x1f, 2, -1, -1, 7);
  if (ox1 != x1)
    wrtltrzoom (x1, 0, 0x1f, 2, opt_col_p1, -1, 7);

  if ((ox2 != ~0) && (ox2 != x2))
    wrtltrzoom (ox2, 0, 0x1f, 2, -1, -1, 7);
  if (ox2 != x2)
    wrtltrzoom (x2, 0, 0x1f, 2, opt_col_p2, -1, 7);

  ox1 = x1;
  ox2 = x2;
}


void
showscore (p)
    struct playa *p;
{
  char s[64];
  sprintf (s, "p0: angle = %3d, force = %3d, lives = %d", p[0].a, p[0].p, p[0].l);
  wrtwrd (0, 448, s, 0, opt_col_p1, 7);
  sprintf (s, "p1: angle = %3d, force = %3d, lives = %d", p[1].a, p[1].p, p[1].l);
  wrtwrd (0, 464, s, 0, opt_col_p2, 7);
}


void
checkkeys (p)
    struct playa *p;
{
  int n = 0;
  static int ps = 0, pe = 0;

  him_keyupd();

  if (him_keypr (SCANCODE_Q))
    {
      p[0].a = (p[0].a + 1) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_R))
   {
      p[0].a = (p[0].a + 255) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_E))
    {
      p[0].p = (p[0].p + 1) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_W))
    {
      p[0].p = (p[0].p + 255) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_A))
    {
      p[0].a = (p[0].a + 4) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_F))
   {
      p[0].a = (p[0].a + 252) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_D))
    {
      p[0].p = (p[0].p + 4) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_S))
    {
      p[0].p = (p[0].p + 252) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_Z))
    {
      p[0].a = (p[0].a + 16) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_V))
   {
      p[0].a = (p[0].a + 240) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_C))
    {
      p[0].p = (p[0].p + 16) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_X))
    {
      p[0].p = (p[0].p + 240) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_T))
    {
      p[0].a = (128 - p[0].a);
      n++;
    }
  
  if (him_keypr (SCANCODE_U))
    {
      p[1].a = (p[1].a + 1) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_P))
   {
      p[1].a = (p[1].a + 255) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_O))
    {
      p[1].p = (p[1].p + 1) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_I))
    {
      p[1].p = (p[1].p + 255) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_H))
    {
      p[1].a = (p[1].a + 4) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_L))
   {
      p[1].a = (p[1].a + 252) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_K))
    {
      p[1].p = (p[1].p + 4) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_J))
    {
      p[1].p = (p[1].p + 252) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_B))
    {
      p[1].a = (p[1].a + 16) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_COMMA))
    {
      p[1].a = (p[1].a + 240) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_M))
    {
      p[1].p = (p[1].p + 16) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_N))
    {
      p[1].p = (p[1].p + 240) % 256;
      n++;
    }
  if (him_keypr (SCANCODE_Y))
    {
      p[1].a = (128 - p[1].a);
      n++;
    }

  if (n)
    {
      paint_tanx (p[0].x, p[0].a, p[1].x, p[1].a);
      showscore (p);
    }

  if (him_keypr (SCANCODE_ESCAPE))
    p[0].l = p[1].l = 0;

  if (him_keypr (SCANCODE_ENTER))
    pe++;
  else
    pe = 0;

  if (him_keypr (SCANCODE_SPACE))
    ps++;
  else
    ps = 0;

  if (pe == 1)
    fire (p, 1);

  if (ps == 1)
    fire (p, 0);
}


struct shootp *
fire (p, n)
    struct playa *p;
    int n;
{
  float vx, vy, bx, by;
  int y, e;
  struct shootp *foo;
  struct shotp *bar;

  e = min (p[n].e, p[n].p);
  p[n].e -= e;

  y = (getwl (p[n].x) ? min (lmhs[p[n].x] - 8, getwl (p[n].x)) : (lmhs[p[n].x] - 8));

  bx = (float) (p[n].x + (cos (p[n].a * PI / 128.0) * 16));
  by = (float) (y - (sin (p[n].a * PI / 128.0) * 16));

  vx = cos (p[n].a * PI / 128.0) * (e / 256.0);
  vy = - sin (p[n].a * PI / 128.0) * (e / 256.0);

  bar = (struct shotp *) malloc (sizeof (struct shotp));
  bar->x = bx;
  bar->y = by;
  bar->nxt = NULL;

  foo = (struct shootp *) malloc (sizeof (struct shootp));
  foo->shon = 1;
  foo->hit = 0;
  foo->vx = vx;
  foo->vy = vy;
  foo->nxt = shoo;
  foo->shob = foo->shol = bar;
  foo->who = n;

  return (shoo = foo);
}


int
lndrnd (int d) 	/* got time since expl, ret 1/0 - the more time since the surer */
{
	int perc, percr;
	perc = d / 15;
	percr = rand () % 100;
	return (percr < perc);
}


void
do_we (p)
   struct playa *p;
{
  p[0].e += opt_rwe;
  p[0].e = (p[0].e > opt_mwe) ? opt_mwe : p[0].e;
  p[1].e += opt_rwe;
  p[1].e = (p[1].e > opt_mwe) ? opt_mwe : p[1].e;
}

	
void
do_fire (p)
   struct playa *p;
{
  struct shootp *hsoo;

  /* fire */
  hsoo = shoo;
  while (hsoo != NULL)
    {
      if (!(hsoo->hit))
        {
          int foo;
          float bx, by;
          float vx, vy;

          bx = hsoo->shol->x;
          by = hsoo->shol->y;
          vx = hsoo->vx;
          vy = hsoo->vy;

          bx += vx; 
          by += vy; 

          if (opt_varg)
            vy += (0.002 * ((SCR_X - lmhs[(int) bx]) / ((double) SCR_X)));
          else
            vy += 0.001;
    
          if (opt_airres)
            {
              vx *= 0.9995;
              vy *= 0.9995;
            }
    
          if (bx >= SCR_X)
            {
              if (opt_mirror)
                vx = -vx;
              else
                hsoo->hit = 2;
            }

          if (bx < 0)
            {
              if (opt_mirror)
                vx = -vx;
              else
                hsoo->hit = 2;
            }

          if (by >= SCR_Y)
            {
              hsoo->hit = 2;
              vy = -vy;
            }

          if (by < 0) 
            {
              if (opt_mirror0)
                vy = -vy;
              else
                {
                  him_pixel ((int) bx, 0, opt_col_shot1, 6);
                  addshpx (hsoo, bx, 0.0);
                }
            }
          else
            {
              him_pixel ((int) bx, (int) by, opt_col_shot0, 6);
              addshpx (hsoo, bx, by);
            }
  
          foo = him_getpixel ((int) (bx + vx), (int) (by + vy), -2);

          if (foo < 1)
            foo = 0;
      
          foo = foo & 0x36;
      
          if (foo & 2)  /* land & water */
            {
              if (him_getpixel ((int) (bx + vx), (int) (by + vy), 1) == opt_col_land)
                {
                  int v;
                  v = (int) (sqrt (sqr (vx) + sqr (vy)) * 20) + 2;
                  addexplpnt ((int) (bx + vx), (int) (by + vy), v);
                  hsoo->hit = 2;
                }
              else
                {
                  int bar = lmhs[(int) (bx + vx)] - getwl ((int) (bx + vx));
                  vx *= 0.99 - (((float) bar) / 24000.0);
                  vy *= 0.99 - (((float) bar) / 24000.0);
                }  
            }

          if (foo & 4)  /* clouds */
            {
              int a;
              struct spos *b;
              unsigned int d;
              
              if (him_getpixel ((int) (bx + vx), (int) (by + vy), 2) == opt_col_cloud)
                {
                  vx *= 0.985;
                  vy *= 0.985;
                }
              else
                {
                  vx *= 0.996;
                  vy *= 0.996;
                }  
              
              /* lightning */
              if (opt_cloudangry)
                {
                  b = c;
                  d = ~0;
                  for (a = 0; a < nc; a++)
                    if (near (c[a].x, c[a].y, (int) (bx + vx), (int) (by + vy)) < d)
                      {
                        b = c + a;
                        d = near (b->x, b->y, (int) (bx + vx), (int) (by + vy));
                      }
                  if (++(b->a) >= opt_cloudangry)
                    {
                      newlight (b->x, b->y, hsoo->who);
                      b->a = 0;
                    }
                }
            }

          if (foo & 32)  /* tanks */
            {
              hsoo->hit = 1;
              if (him_getpixel ((int) (bx + vx), (int) (by + vy), 5) == opt_col_p1)
                {
                  p[0].x = (rand () % 288) + 16;
                  p[0].l = p[0].l - 1;
                }
              else
                {
                  p[1].x = (rand () % 288) + 336;
                  p[1].l = p[1].l - 1;
                }
              paint_tanx (p[0].x, p[0].a, p[1].x, p[1].a);
              show_arrows (p[0].x, p[1].x);
              showscore (p);
              addexplpnt ((int) (bx + vx), (int) (by + vy), 16);
            }
          
          hsoo->vx = vx;
          hsoo->vy = vy;
        }
      
      if ((hsoo->shon > 128) || (hsoo->hit && hsoo->shon))
        {  
          him_pixel ((int) hsoo->shob->x, (int) hsoo->shob->y, -1, 6);
          hsoo->shob = frees (hsoo->shob, &(hsoo->shon));
        }
 
      hsoo = hsoo->nxt;
    } /* end of fire */
}


void
do_expl (p)
   struct playa *p;
{
  struct expl *exxp;

  /* explosion */
  exxp = expb;
  while ((exxp != NULL) && (expb != NULL))
    {
      exxp->c += (((float) exxp->p) / 128.0);
      him_ufullcircler (exxp->x, exxp->y, (int) exxp->d, (int) exxp->c, -1, 1, &px_hole);

      exxp->d = exxp->c;

      if ((exxp->c) >= ((float) exxp->p))
        {
          struct expl *exxxp;
 
          if ((abs (exxp->x - p[0].x) < 24) || (abs (exxp->x - p[1].x) < 24))
            {
              paint_tanx (p[0].x, p[0].a, p[1].x, p[1].a);
              show_arrows (p[0].x, p[1].x);
              showscore (p);
            }

          if (exxp->pre != NULL)
            exxp->pre->nxt = exxp->nxt;
          
          if (exxp->nxt != NULL)
            exxp->nxt->pre = exxp->pre;
            
          if (exxp == expb)
            expb = expb->nxt;
          
          exxxp = exxp->nxt;
          free (exxp);
          exxp = exxxp;
        }
      else
        exxp = exxp->nxt;
    } /* end of explosion */
}  


void
do_light (p)
   struct playa *p;
{
  struct lghtt *lt;

  /* lightning */
  lt = lghtb;
  while (lt != NULL)
    {
      struct lght *cur;
      int foo = 0;

      cur = lt->ls;
      lt->ls = NULL;

      while ((cur != NULL) && (cur != lt->ls))
        if (cur->h)
          cur = cur->n;
        else
          {
            int bar;
          
            cur->nc = ((lt->numforks < opt_max_ltforks) ? ((rand () % 3) + 1) : 0);
            cur->c = (struct lght **) calloc (cur->nc, sizeof (struct lght *));
            
            for (bar = 0; bar < cur->nc; bar++)
              {
                struct lght *fux;
                int d, px, py;
                double a;
                
                lt->numforks++;        
              
                cur->c[bar] = fux = (struct lght *) malloc (sizeof (struct lght));
                if ((foo == 0) && (bar == 0))
                  lt->ls = fux;

                px = p[lt->w].x;
                py = (getwl (px) ? min(lmhs[px] - 8, getwl (px)) : (lmhs[px] - 8));
                
                d = near (cur->x, cur->y, px, py);
                a = acos (((float) (abs (cur->y - py) * ((cur->x > px) ? (-1) : (1)))) / ((float) d));
              
                a += (((float) ((rand () % 101) - 50)) / 200.0) * (1.0 + (16.0 / (float) (d + 1)));
                if (d < 32)
                  d *= 2;
                d /= 3;
                 
                fux->h = 0;
                fux->nc = 0;

                if (!lt->h)
                  {
                    fux->x = cur->x + abs (roundH (sin (a) * ((float) d))) * (((a > (PI / 2.0)) && (a < (3.0 * PI / 2.0))) ? (-1) : 1);
                    fux->y = cur->y + abs (roundH (cos (a) * ((float) d))) * (((a > PI) || (a < 0)) ? (-1) : 1);
                  }
                else
                  {
                    fux->x = cur->x + (rand () % 31) - 15;
                    fux->y = cur->y + (rand () % 31) - 15;
                  }
                  
                if (fux->x < 0)
                  {
                    fux->x = 0;
                    fux->h = 1;
                  }
                if (fux->y < 0)
                  {
                    fux->y = 0;
                    fux->h = 1;
                  }
                if (fux->x >= SCR_X)
                  {
                    fux->x = SCR_X - 1;
                    fux->h = 1;
                  }
                if (fux->y >= SCR_Y)
                  {
                    fux->y = SCR_Y - 1;
                    fux->h = 1;
                  }
              
                fux->p = lt->lp;
                fux->n = (struct lght *) NULL;
                lt->lp->n = fux;
                lt->lp = fux;
                
                fux->u = cur;
                fux->c = (struct lght **) NULL;
              
                if (him_getpixel (fux->x, fux->y, 1) == opt_col_land)
                  {
                    fux->h = 1;
                    addexplpnt (fux->x, fux->y, 8);
                  }
                else if (him_getpixel (fux->x, fux->y, 5) > 0)
                  {
                    fux->h = 1;
                    if (him_getpixel (fux->x, fux->y, 5) == opt_col_p1)
                      {
                        p[0].x = (rand () % 288) + 16;
                        p[0].l = p[0].l - 1;
                      }
                    else
                      {
                        p[1].x = (rand () % 288) + 336;
                        p[1].l = p[1].l - 1;
                      }
                    paint_tanx (p[0].x, p[0].a, p[1].x, p[1].a);
                    show_arrows (p[0].x, p[1].x);
                    showscore (p);
                    addexplpnt (fux->x, fux->y, 16);
                  }
 
                him_line (cur->x, cur->y, fux->x, fux->y, opt_col_lt, 6);
              }

            foo++;
            cur = cur->n;
          }
      
      if (foo == 0)
        {
          struct lghtt *xx;
          
          light_depaint (lt->lb);
          
          while (lt->lb)
            {
              struct lght *ll;
              ll = lt->lb;
              lt->lb = lt->lb->n;
              free (ll);
            }
 
          if (lt->n != NULL)
            lt->n->p = lt->p;

          if (lt->p != NULL)
            lt->p->n = lt->n;
          
          if (lt == lghtb)
            lghtb = lghtb->n;
            
          xx = lt->n;
          free (lt);
          lt = xx;
        }
      else  
        lt = lt->n;
    }
}


void
do_land (void)
{
  struct lndpts *lnd;

  if (!opt_lndsld)
    return;

  /* land slide */
  
  lnd = lndpt;
  if ((lnd != NULL) && lndrnd(him_getnow() - lnd->tm))
  	lnd = lnd->n;
  
  while (lnd != NULL)
    {
      struct lndpts *ko;
      int foo;		/* used in lndstf macro */
      
#define lndstf(x,y,t)		/* x, y, target x delta */	\
      if (((x) > 0) && ((x) < SCR_X - 1)			\
      && ((x) + (t) > 0) && ((x) + (t) < SCR_X - 1)		\
      && (him_getpixel ((x), (y), 1) == opt_col_land)		\
      && (((foo = him_getpixel ((x) + (t), (y) + 1, 1)) < 1)	\
      || (him_getpixel ((x) + (t), (y) + 1, 1) == opt_col_wtr))	\
      )								\
        { 							\
          him_pixel ((x), (y), foo, 1);				\
          him_pixel ((x) + (t), (y) + 1, opt_col_land, 1);	\
          addlndpt ((x) + (t), (y) + 1); 			\
          if (him_getpixel ((x), (y) - 1, 1) == opt_col_land) 	\
            addlndpt ((x), (y) - 1); 				\
        }
          
      lndstf(lnd->x, lnd->y, 0);

      if (rand() % 2) {
	lndstf(lnd->x, lnd->y, 1)
          else
	lndstf(lnd->x, lnd->y, -1);
      } else {
 	lndstf(lnd->x, lnd->y, -1)
          else
        lndstf(lnd->x, lnd->y, 1);
      }
        
      if (lnd->p != NULL)
        lnd->p->n = lnd->n;
      if (lnd->n != NULL)
        lnd->n->p = lnd->p;
      if (lndpt == lnd)
        lndpt = lnd->n;
      
      ko = lnd;
      lnd = lnd->n;
      free (ko);
      
      if ((lnd != NULL) && lndrnd(him_getnow() - lnd->tm))
        lnd = lnd->n;
    }
}


struct shotp
*frees (n, i)
    struct shotp *n;
    int *i;
{
  struct shotp *m;
 
  if (n == NULL)
    return NULL;
 
  m = n->nxt;
  free (n);
  *i -= 1;
  return m;
}


struct shootp
*freeso (n)
    struct shootp *n;
{
  struct shootp *m;
 
  if (n == NULL)
    return NULL;
 
  m = n->nxt;
  while ((n->shob = frees (n->shob, &(n->shon))) != NULL);
  free (n);

  return m;
}



void
addshpx (d, x, y)
    double x, y;
    struct shootp *d;
{
  struct shotp *foo;

  foo = (struct shotp *) calloc (1, sizeof (struct shotp));
  if (foo == NULL)
    return;
    
  foo->nxt = NULL;

  if (d->shol != NULL)
    d->shol->nxt = foo;

  d->shol = foo;
  
  if (d->shob == NULL)
    d->shob = foo;

  foo->x = x;
  foo->y = y;
  ++(d->shon);
}


void
addexplpnt (x, y, p)
     int x, y, p;
{
  struct expl *a;

  if (x >= SCR_X)
    x = SCR_X - 1;
  if (x < 0)
    x = 0;
  if (y >= SCR_Y)
    y = SCR_Y - 1;
  if (y < 0)
    y = 0;
    
  a = (struct expl *) malloc (sizeof (struct expl));  
  if (a == NULL)
    return;

  a->nxt = expb;
  if (expb != NULL)
    expb->pre = a;
  a->pre = NULL;
  a->x = x;
  a->y = y;
  a->p = p;
  a->d = a->c = 0.0;
  expb = a;
}


int
bezier (x, p, px, py, t)
    int x, p, t;  /* x = 0..t; p = 1..MAXINT (num points) */
    int *px, *py;
{
  float *bx, *by;
  int foo;
  
  if (p == 0)
    return ~0;

  bx = (float *) malloc (p * sizeof (float));
  by = (float *) malloc (p * sizeof (float));

  for (foo = 0; foo < p; foo++)
    {
      bx[foo] = (float) px[foo];
      by[foo] = (float) py[foo];
    }
  
  for (foo = p - 1; foo > 0; foo--)
    {
      int bar;
      
      for (bar = 0; bar < foo; bar++)
        {
          bx[bar] = bx[bar] + ((bx[bar + 1] - bx[bar]) * ((float) x / (float) t));
          by[bar] = by[bar] + ((by[bar + 1] - by[bar]) * ((float) x / (float) t));
        }
    }

  return ((int) by[0]);
}


int
gplnd (x, y, c, l)
     int x, y, c, l;
{
  static int lx = ~0;

  if (lx != x)
    {
      lmhs[x] = y;
      for (lx = y; lx < SCR_Y; lx++)
        if (him_pixel (x, lx, c, l) == -1)
          {
            lx = x;
            return -1;
          }
    }

  lx = x;
  return 0;
}


int
px_hole (x, y, c, l)
    int x, y, c, l;
{
  int foo;
 
  foo = him_pixel (x, y, c, l);
  wtr_hole (x, y);
  
  if (foo != 0)
    return foo;
  
  if (him_getpixel (x, y - 1, 1) == opt_col_land)
    addlndpt (x, y - 1);

  while (him_getpixel (x, y++, 1) < 1)
    if (y >= SCR_Y)
      {
        y = SCR_Y - 1;
        break;
      }
     
  lmhs[x] = y - 1;
  
  return 0;
}


int
px_cloud (x, y, c, l)
     int x, y, c, l;
{
  int xx, yy, n = 0;
  
  if (!((x + y) % 2))
    return 0;
  
  xx = x + (rand () % 3) - 1;
  yy = y + (rand () % 3) - 1;
  
  n += him_pixel (xx, yy, c, l);
  n += him_pixel (xx + 1, yy, opt_col_ca, l);
  n += him_pixel (xx - 1, yy, opt_col_ca, l);
  n += him_pixel (xx, yy + 1, opt_col_ca, l);
  n += him_pixel (xx, yy - 1, opt_col_ca, l);
  
  return n;
}


void
cloud (cc)
    struct spos *cc;
{
  int foo, *px, *py;
  float sc;
  
  cc->x = (rand () % 384) + 128;
  cc->a = 0;
  
  foo = (getwl (cc->x) ? (getwl (cc->x)) : (lmhs[cc->x]));
  if (foo < 64)
    {
      cc->y = foo / 2;
      sc = ((float) foo) / 64.0;
    }
  else
    {
      cc->y = (rand () % (foo - 64)) + 32;
      sc = 1;
    }

  px = (int *) malloc (8 * sizeof (int));
  py = (int *) malloc (8 * sizeof (int));

  if ((px == NULL) || (py == NULL))
    {
      free (c);

      letters_destroy ();   /* free font */
      him_destroy ();       /* free graphics */
      exit (1);
    }

  px[0] = 0;
  py[0] = 0;
  px[1] = (rand () % 32);
  py[1] = (rand () % 48);
  px[2] = (rand () % 32) + 16;
  py[2] = (rand () % 32) + 16;
  px[3] = (rand () % 32) + 32;
  py[3] = (rand () % 48);
  px[4] = (rand () % 32) + 64;
  py[4] = (rand () % 48);
  px[5] = (rand () % 32) + 80;
  py[5] = (rand () % 32) + 16;
  px[6] = (rand () % 32) + 96;
  py[6] = (rand () % 48);
  px[7] = 127;
  py[7] = 0;

  if (sc != 1)
    for (foo = 0; foo < 8; foo++)
      py[foo] = (int) (((float) py[foo]) * sc);

  for (foo = 0; foo < 64; foo++)
    {
      int bar = bezier (foo * 2, 8, px, py, 127);
      him_uline (cc->x + (foo * 2) - 64, cc->y - bar, cc->x + (foo * 2) - 64, cc->y + bar, opt_col_cloud, 2, &px_cloud);
    }

  free (px);
  free (py);
}


int
px_moon (x, y, c, l)
     int x, y, c, l;
{
  c = rand () % 16;
  if (c < 8)
    c = 15;
  else if (c < 12)
    c = 7;
  else c = 8;
  return him_pixel (x, y, c, l);
}


void
newlight (x, y, w)
    int x, y, w;
{
  struct lghtt *foo;

  foo = (struct lghtt *) malloc (sizeof (struct lghtt));
  foo->w = w;
  foo->h = 0;
  foo->p = (struct lghtt *) NULL;
  foo->n = lghtb;
  foo->numforks = 1;

  foo->lb = foo->ls = foo->lp = (struct lght *) malloc (sizeof (struct lght));
  foo->lb->x = x;
  foo->lb->y = y;
  foo->lb->h = 0;
  foo->lb->nc = 0;
  foo->lb->n = foo->lb->p = foo->lb->u = (struct lght *) NULL;
  foo->lb->c = (struct lght **) NULL;

  lghtb = foo;
}


void
light_depaint (l)
    struct lght *l;
{
  int foo;

  for (foo = 0; foo < l->nc; foo++)
    {
      light_depaint (l->c[foo]);
      him_line (l->x, l->y, l->c[foo]->x, l->c[foo]->y, -1, 6);
    }
}


void
addlndpt (x, y)
    int x, y;
{
  struct lndpts *nw;

  if (!opt_lndsld)
    return;

  nw = (struct lndpts *) malloc (sizeof (struct lndpts));

  nw->p = nw->n = (struct lndpts *) NULL;

  if (lndpt != NULL)
    {
      lndpt->p = nw;
      nw->n = lndpt;
    }

  lndpt = nw;
  nw->x = x;
  nw->y = y;
  nw->tm = him_getnow();
}


void showebar (int p0, int p1)
{
	static int a = 0, b = 0;
	if (p0 != a) {
		him_box (SCR_X - 1 - (p0 * 128 / opt_mwe), SCR_Y - 32, SCR_X - 1,                        SCR_Y - 17, opt_col_p1, 7);
		him_box (SCR_X - 128,                      SCR_Y - 32, SCR_X - 2 - (p0 * 128 / opt_mwe), SCR_Y - 17, -1, 7);
		a = p0;
	}
	if (p1 != b) {
		him_box (SCR_X - 1 - (p1 * 128 / opt_mwe), SCR_Y - 16, SCR_X - 1,                        SCR_Y - 1, opt_col_p2, 7);
		him_box (SCR_X - 128,                      SCR_Y - 16, SCR_X - 2 - (p1 * 128 / opt_mwe), SCR_Y - 1, -1, 7);
		b = p1;
	}
}
