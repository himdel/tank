// general routines

export const distance = (dx, dy) => Math.sqrt((dx ** 2) + (dy ** 2));

export const squares_diff = (dx, dy) => Math.sqrt((dx ** 2) - (dy ** 2));

export const near = (x1, y1, x2, y2) => distance(Math.abs(x2 - x1), Math.abs(y2 - y1));
