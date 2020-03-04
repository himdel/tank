// general routines for fonted

export const distance = (dx, dy) => Math.sqrt((dx ** 2) + (dy ** 2));

export const squares_diff = (dx, dy) => Math.sqrt((dx ** 2) - (dy ** 2));

double
near (x1, y1, x2, y2)
    int x1, y1, x2, y2;
{
  int dx, dy;

  dx = abs (x2 - x1);
  dy = abs (y2 - y1);

  return distance(dx, dy);
}
