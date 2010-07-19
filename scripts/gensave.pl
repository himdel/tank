# generates options to save from options.h as STDIN
# use: perl gensave.pl < options.h

while (<STDIN>)
  {
    if (/extern int/)
      {
	chomp;
        s/^extern int opt_(.*?);.*$/$1/;
        print "  fprintf (fp, \"opt_$_ = \%d\\n\", opt_$_);\n";
      }
  }
