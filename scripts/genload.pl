# generates options to load from options.h as STDIN
# use: perl genload.pl < options.h

$y = 0;

while (<STDIN>)
  {
    if (/extern int/)
      {
	chomp;
	
        s/^extern int opt_(.*?);.*$/$1/;

        print "      ";
   
        ($y == 1) && print "else ";
        $y = 1;
   
        print "if (strstr (fn, \"opt_$_\") != NULL)\n        opt_$_ = atoi (\&fn [strlen (\"opt_$_\") + 3]);\n";
      }
  }
