// general routines for fonted

double sqr (double);
double near (int, int, int, int);
int max (int, int);
int min (int, int);




double
sqr (n)
    double n;
{
  return (n * n);
}


double
near (x1, y1, x2, y2)
    int x1, y1, x2, y2;
{
  int dx, dy;

  dx = abs (x2 - x1);
  dy = abs (y2 - y1);

  return sqrt (sqr ((double) dx) + sqr ((double) dy));
}


int
max (a, b)
    int a, b;
{
  return ((a > b) ? a : b);
}


int
min (a, b)
    int a, b;
{
  return ((a < b) ? a : b);
}
