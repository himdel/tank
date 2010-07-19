/*
 options.h - options for tank
 */


extern int opt_lndpts;
extern int opt_jmp;
extern int opt_mirror;
extern int opt_mirror0;
extern int opt_airres;
extern int opt_varg;
extern int opt_col_land;
extern int opt_col_cloud;
extern int opt_col_ca;
extern int opt_col_p1;
extern int opt_col_p2;
extern int opt_col_shot0;
extern int opt_col_shot1;
extern int opt_col_star;
extern int opt_col_wtr;
extern int opt_col_sun;
extern int opt_col_moon;
extern int opt_col_lt;
extern int opt_ilives;
extern int opt_cloudangry;
extern int opt_num_stars;
extern int opt_v;
extern int opt_lndsld;
extern int opt_max_ltforks;
extern int opt_rewater;
extern int opt_iwe;	/* initial weapon energy */
extern int opt_mwe;	/* max weap energy */
extern int opt_rwe;	/* recharge weap energy per sec */

void opt_init (void);
void opt_save (void);
