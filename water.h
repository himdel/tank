/*
  water.h - water routines for tank
 */


void water_init (void);
void water_destroy (void);

void water_land (int *);
void rewater (void);
int getwl (int);
void wtr_hole (int, int);
