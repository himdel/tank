#include <stdio.h>
#include "../paint.h"

int
main ()
{
  int foo;
  
  him_init (640, 480, 16, 1, 0);
  
  for (foo = 0; foo < 16; foo++)
    {
      int r, g, b;
      him_getpalette (foo, &r, &g, &b);
      printf ("%d: %d, %d, %d\n", foo, r * 4, g * 4, b * 4);
    }
  
  him_destroy ();
}
