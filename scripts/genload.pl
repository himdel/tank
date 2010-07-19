#!/usr/bin/perl
# generates options to load from options.h as STDIN
# use: perl genload.pl < options.h

my $y = 0;

while (<>) {
	chomp;
	s/^extern int opt_(.*?);.*$/$1/ or next;
	print "\t\t";
	print "else " if $y;
	$y = 1;

	print "if (strstr(fn, \"opt_$_\"))\n";
	print "\t\t\topt_$_ = atoi(fn + strlen(\"opt_$_\") + 3);\n";
}

print "\t\telse if (strlen(fn) > 1)\n";
print "\t\t\tprintf(\"tank: Unknown option %s -IGNORING\\n\", fn);\n";
