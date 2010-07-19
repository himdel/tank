/*
  letters.h - letter writing routines for tank
 */

extern int letters_fontH;


/* filename or NULL to use default font */
int letters_init (char *);
void letters_destroy ();

/* x, y, char//string/num, [zoom], fgcolor, bgcolor, layer */
void wrtltr (int, int, int, int, int, int);
void wrtltrzoom (int, int, int, int, int, int, int);
void wrtwrd (int, int, char*, int, int, int);
void wrtint (int, int, int, int, int, int);
