/*
  menu.c - menu for tank
  himdel@seznam.cz
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "paint.h"
#include "options.h"
#include "letters.h"
#include "menu.h"
#include "gui.h"

int menu_p0 = 0;
int menu_p1 = 0;

int mnu_settings ();
int mnu_network ();



int
startmenu (ait)
  int ait;
{
  int foo;

  him_clrscr ();  
  
  him_putlock ();
  
  if (him_showimg ("menu.png") >= 0)
    {
      int x, y;
      
      him_setpalette (0, 0, 0, 0);
      him_setpalette (4, 0xa8, 0, 0);
      him_setpalette (12, 0xfc, 0x54, 0x54);
      
      for (x = 256; x < 384; x++)
        for (y = 160; y < 320; y++)
          if ((x + y) % 2)
            him_putpixel (x, y, 0);
      
      wrtwrd (320 - 8, 172, "1v1", 0, -2, -1);
      wrtwrd (320 - 12, 204, "1vPC", 0, -2, -1);
      wrtwrd (320 - 16, 236, "1vNET", 0, -2, -1);
      wrtwrd (320 - 28, 268, "settings", 0, -2, -1);
      wrtwrd (320 - 12, 300, "quit", 0, -2, -1);

      wrtwrd (320 - 14, 166, "1v1", 0, -2, -1);
      wrtwrd (320 - 18, 198, "1vPC", 0, -2, -1);
      wrtwrd (320 - 22, 230, "1vNET", 0, -2, -1);
      wrtwrd (320 - 34, 262, "settings", 0, -2, -1);
      wrtwrd (320 - 18, 294, "quit", 0, -2, -1);

      him_filledtriangle (256, 168, 256, 168 + 16, 268, 168 + 8, 0, -1);
      him_filledtriangle (256, 200, 256, 200 + 16, 268, 200 + 8, 0, -1);
      him_filledtriangle (256, 232, 256, 232 + 16, 268, 232 + 8, 0, -1);
      him_filledtriangle (256, 264, 256, 264 + 16, 268, 264 + 8, 0, -1);
      him_filledtriangle (256, 296, 256, 296 + 16, 268, 296 + 8, 0, -1);
    }

  wrtwrd (320 - 10, 170, "1v1", 4, -2, -1);
  wrtwrd (320 - 14, 202, "1vPC", 4, -2, -1);
  wrtwrd (320 - 18, 234, "1vNET", 4, -2, -1);
  wrtwrd (320 - 30, 266, "settings", 4, -2, -1);
  wrtwrd (320 - 14, 298, "quit", 4, -2, -1);
  
  wrtwrd (320 - 12, 168, "1v1", 12, -2, -1);
  wrtwrd (320 - 16, 200, "1vPC", 12, -2, -1);
  if (him_is_SDL)
    wrtwrd (320 - 20, 232, "1vNET", 12, -2, -1);
  wrtwrd (320 - 32, 264, "settings", 12, -2, -1);
  wrtwrd (320 - 16, 296, "quit", 12, -2, -1);

  foo = 168 + (((ait + 4) % 5) * 32); 
  him_filledtriangle (256, foo, 256, foo + 16, 268, foo + 8, 12, -1);

  him_putulock ();
  him_putupd ();

  while (1)
    {
      static int u = 0, d = 0;  /* up, down */

      him_keyupd ();
      
      if (him_keypr (SCANCODE_ESCAPE))
        return 0;
      
      if (him_keypr (SCANCODE_ENTER))
        {
          do
            {
              him_keyupd ();
            }
          while (him_keypr (SCANCODE_ENTER));

          switch (ait)
            {
              case 0:
                return 0;
                break;
              case 1:
                menu_p0 = 0;
                menu_p1 = 0;
                return 1;
                break;
              case 2:
                menu_p0 = 0;
                menu_p1 = 2;
                return 2;
                break;
              case 3:
                if ((foo = mnu_network ()))
                  {
                    menu_p0 = menu_p1 = 0;
                    if (foo == 1)
                      menu_p0 = 1;
                    else if (foo == 2)
                      menu_p1 = 1;
                    else
                      {
                        printf ("guru medication 0x01\n");
                        return 0;
                      }
                    return 3;
                  }
                break;
              case 4:
                if (mnu_settings ())
                  opt_save ();
                break;
              default:
                printf ("guru medication 0x00\n");
                return 0;
            }
        }
      
      if (d && (!him_keypr (SCANCODE_CURSORBLOCKDOWN)))
        d = 0;
      if (u && (!him_keypr (SCANCODE_CURSORBLOCKUP)))
        u = 0;
      
      if ((d == 0) && (him_keypr (SCANCODE_CURSORBLOCKDOWN)))
        {
          him_putlock ();
          
          d = 1;
          
          foo = 168 + (((ait + 4) % 5) * 32); 
          him_filledtriangle (256, foo, 256, foo + 16, 268, foo + 8, 0, -1);

          ait = ((ait + 1) % 5);    
          if ((him_is_SDL == 0) && (ait == 3))
            ait = ((ait + 1) % 5);

          foo = 168 + (((ait + 4) % 5) * 32); 
          him_filledtriangle (256, foo, 256, foo + 16, 268, foo + 8, 12, -1);
 
          him_putulock ();
          him_putupd ();
        }

      if ((u == 0) && (him_keypr (SCANCODE_CURSORBLOCKUP)))
        {
          him_putlock ();

          u = 1;
          
          foo = 168 + (((ait + 4) % 5) * 32); 
          him_filledtriangle (256, foo, 256, foo + 16, 268, foo + 8, 0, -1);
          
          ait = ((ait + 4) % 5);
          if ((him_is_SDL == 0) && (ait == 3))
            ait = ((ait + 4) % 5);

          foo = 168 + (((ait + 4) % 5) * 32); 
          him_filledtriangle (256, foo, 256, foo + 16, 268, foo + 8, 12, -1);

          him_putulock ();
          him_putupd ();
        }
    }

  return 0;
}



int
mnu_network ()
{
  int x, y;
  
  him_putlock ();
  him_showimg ("netgame.png");
      
  for (x = 200; x < 440; x++)
    for (y = 212; y < 272; y++)
      him_putpixel (x, y, 0);
      
  wrtwrd (208, 216, "connect to: ", 2, -2, -1);
  wrtwrd (208, 248, "be server", 1, -2, -1);

  him_box (304, 215, 432, 232, 1, -1);
  him_box (305, 216, 431, 231, 0, -1);

  him_putulock ();
  him_putupd ();

/* TODO: act as edit ten ramecek - zadas IP, ta se da do SDLNet_ResolveHost, ... */
//  SDLNet_Init ();

sleep (5);
//  SDLNet_Quit ();

  return 0;
}


int
mnu_settings ()
{
  printf ("TODO: mnu_settings ()\n");
  return 0;
}
