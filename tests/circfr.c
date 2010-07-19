#include "../paint.h"

int
main ()
{
  him_init (640, 480, 16, 1, 1);
  him_dirmode = 1;
  
  him_ufullcircler (320, 240, 16, 32, 14, 0, &him_pixel);
  
  while (him_getkey () == 0);
  exit (0);
}
