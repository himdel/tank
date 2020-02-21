// general routines for fonted

void reverse (char *, int);
void getstr (char *, int);
int roundH (double);
double sqr (double);
double near (int, int, int, int);
int max (int, int);
int min (int, int);
int hgetline (FILE *, char *, int);


void
reverse (s, l)
     char *s;
     int l;
{
  int foo;
  for (foo = 0; foo < (l / 2); foo ++)
    {
      char z;
      z = s[foo];
      s[foo] = s[l - 1 - foo];
      s[l - 1 - foo] = z;
    }
}


void
getstr (s, l)
     char *s;
     int l;
{
  int c;
  while (((c = getchar ()) != EOF) && (c != '\n') && l--)
    *(s++) = c;
  *s = 0;
}


int
roundH (f)
    float f;
{
  return ((f < (((int) f) + 0.5)) ? ((int) f) : (((int) f) + 1));
}


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


int
hgetline (fp, s, max)
    FILE *fp;
    char *s;
    int max;
{
  char c;

  s[max] = 0;
  c = fgetc (fp);
  while ((c != EOF) && (c != '\n'))
    {
      if (max--)
        *(s++) = c;
      c = fgetc (fp);
    }
  *s = 0;

  return c;
}
