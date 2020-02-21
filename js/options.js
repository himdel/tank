// options - options for tank

int opt_lndpts;
int opt_jmp;
int opt_mirror;
int opt_mirror0;
int opt_airres;
int opt_varg;
int opt_col_land;
int opt_col_cloud;
int opt_col_ca;
int opt_col_p1;
int opt_col_p2;
int opt_col_shot0;
int opt_col_shot1;
int opt_col_star;
int opt_col_wtr;
int opt_col_sun;
int opt_col_moon;
int opt_col_lt;
int opt_ilives;
int opt_cloudangry;
int opt_num_stars;
int opt_v;
int opt_lndsld;
int opt_max_ltforks;
int opt_rewater;
int opt_iwe;  // initial weapon energy
int opt_mwe;  // max weap energy
int opt_rwe;  // recharge weap energy per sec


export function opt_init() {
  opt_lndpts = (rand() % 16) + 16;  // # of bezier points that define the land
  opt_jmp = 8;  // every #th point is beziered, the rest is simply lined
  opt_mirror = 1;  // (bool) shots mirror from the side walls
  opt_mirror0 = 0;  // (bool) " up wall
  opt_airres = 0;  // shots are subjects to the resistance of air
  opt_varg = 0;  // (bool) variable gravity (depending on the height of land)
  opt_ilives = 5;  // # lives
  opt_cloudangry = 2048; // # of cloudpixels to touch to get a cloud angry (0 - disable)
  opt_num_stars = 256; // # of stars
  opt_v = 2; // (bool) verbose
  opt_lndsld = 1; // (bool) land slides
  opt_max_ltforks = 128; // max number of forks for 1 lightning
  opt_rewater = 1; // (bool) activates adjusting water after each explosion
  opt_iwe = 256; // init weap energy
  opt_mwe = 512; // max
  opt_rwe = 16; // recharge per sec

  // 15 white       07 lightgray
  // 14 yellow      06 brown
  // 13 lightviolet 05 violet
  // 12 lightred    04 red
  // 11 cyan        03 darkcyan
  // 10 lightgreen  02 green
  // 09 lightblue   01 blue
  // 08 gray        00 black
  opt_col_land = 10;  // lightgreen
  opt_col_cloud = 15;  // white
  opt_col_ca = 7;  // lightgray
  opt_col_p1 = 2;  // green
  opt_col_p2 = 4;  // red
  opt_col_shot0 = 14;  // yellow
  opt_col_shot1 = 6;  // brown
  opt_col_star = 8;  // gray
  opt_col_wtr = 1;  // blue
  opt_col_sun = 14;  // yellow
  opt_col_moon = 15;  // white
  opt_col_lt = 9;  // lightblue
}
