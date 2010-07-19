#!/usr/bin/perl
# generates options to save from options.h as STDIN
# use: perl gensave.pl < options.h

s/^extern int opt_(.*?);.*$/\tfprintf(fp, "opt_$1 = %d\\n", opt_$1);/ and print while <>;
