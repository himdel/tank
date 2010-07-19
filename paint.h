/*
  paint.h - him_painting routines for tank
 */

#include "vgakeyboard.h"

/* screenx, screeny, colors, layers, rawkey (bool) */
int him_init (int, int, int, int, int);
void him_destroy ();

/* x, y, color[, layer] */
int him_pixel (int, int, int, int);
int him_putpixel (int, int, int);

/* x, y, layer (if -1 returns pixel from the highest layer; if -2 return mask) */
int him_getpixel (int, int, int);

/* x1, y1, x2, y2, color, layer, [function to call for each pixel] */
int him_uline (int, int, int, int, int, int, int (*) (int, int, int, int));
int him_line (int, int, int, int, int, int);

/* x1, y1, x2, y2, color, layer */
int him_box (int, int, int, int, int, int);

/* x, y, r, color, layer, [function to call for each pixel] */
int him_circle (int, int, int, int, int);
int him_ufullcircle (int, int, int, int, int, int (*) (int, int, int, int));

/* r1, r2 */
int him_ufullcircler (int, int, int, int, int, int, int (*) (int, int, int, int));

/* (x,y)123, color, layer */
int him_filledtriangle (int, int, int, int, int, int, int, int);

void him_clrscr ();
void him_repaint ();

void him_setpalette (int, int, int, int);
void him_getpalette (int, int *, int *, int *);
void him_getpalvec (int, int, int *);
void him_setpalvec (int, int, int *);


/* keyboard stuff */

void him_keykill ();
int him_getkey ();

/* "raw": */
int him_keyupd (); 
int him_keypr (int);


/* a time in ms since start */
unsigned int him_getnow(void);


void him_putlock (void);
void him_putulock (void);
void him_putupd (void);

