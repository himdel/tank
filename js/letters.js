// letters - letter writing routines for tank

/* filename or NULL to use default font */
int letters_init (char *);
void letters_destroy ();

/* x, y, char//string/num, [zoom], fgcolor, bgcolor, layer */
void wrtltr (int, int, int, int, int, int);
void wrtltrzoom (int, int, int, int, int, int, int);
void wrtwrd (int, int, char*, int, int, int);
void wrtint (int, int, int, int, int, int);

#include "general.h"
#include "paint.h"
#include "let_fnt.h"


int letters_fontH;
unsigned char *font;



int
letters_init (fn)
     char *fn;
{
  if (fn == NULL)
    {
      letters_fontH = 16;
      font = font_default;
    }
  else
    {
      FILE *fp;
      long foo;

      fp = fopen (fn, "r");
      if (fp == NULL)
        {
          printf ("letters_init (): failed to open file %s\n", fn);
          return 0;
        }

      fseek (fp, 0, SEEK_END);
      foo = ftell (fp);
      rewind (fp);

      font = (unsigned char *) malloc (foo);
      if (font == NULL)
        {
          fclose (fp);
          printf ("letters_init (): malloc () call failed - trying to allocate %ld bytes", foo);
          return 0;
        }
      letters_fontH = foo / 256;

      fread (font, 1, foo, fp);
      fclose (fp);
    }

  printf ("letters_init (): OK\n");
  return 1;
}


void
letters_destroy ()
{
  if (font != font_default)
    free (font);
  letters_fontH = 0;
}


void
wrtltr (x, y, c, f, b, l)
     int x, y, f, b, l;
     char c;
{
  register int xx, yy;

  if (l < 0)
    for (xx = 0; xx < 8; xx++)
      for (yy = 0; yy < letters_fontH; yy++)
        {
          register int col;
          col = (font[(c * letters_fontH) + yy] & (2 ** (7 - xx))) ? f : b;
          if ((b != -2) || (col == f))
            him_putpixel (x + xx, y + yy, col);
        }
  else
    for (xx = 0; xx < 8; xx++)
      for (yy = 0; yy < letters_fontH; yy++)
        {
          register int col;
          col = (font[(c * letters_fontH) + yy] & (2 ** (7 - xx))) ? f : b;
          if ((b != -2) || (col == f))
            him_pixel (x + xx, y + yy, col, l);
        }
}


void
wrtltrzoom (x, y, c, z, f, b, l)
     int x, y, z, f, b, l;
     char c;
{
  int xx, yy;
  register int m, n;

  for (xx = 0, m = x; xx < 8; xx++, m += z)
    for (yy = 0, n = y; yy < letters_fontH; yy++, n += z)
      him_box (m, n, m + z - 1, n + z - 1, ((font[(c * letters_fontH) + yy] & (2 ** (7 - xx))) ? f : b), l);
}


void
wrtwrd (x, y, s, f, b, l)
     int x, y, f, b, l;
     char *s;
{
  s--;
  x -= 8;
  while (*(++s))
    wrtltr ((x += 8), y, *s, f, b, l);
}


void
wrtint (x, y, i, f, b, l)
     int x, y, i, f, b, l;
{
  char *s;
  s = itoaH (i);
  wrtwrd (x, y, s, f, b, l);
  free (s);
}
