#!/usr/bin/perl

sub usage {
  print STDERR "Usage: $0 <inputfile> <outputfile>\n";
} # End usage

($#ARGV == 1) || do { usage(); exit(1); };

my $inputfile = shift;
my $outputfile = shift;

open(INPUT, "<$inputfile") || die "Can't open $inputfile for reading: $!\n";
open(OUTPUT, ">$outputfile") || die "Can't open $outputfile for writing: $!\n";

LOOP:
while (my $line=<INPUT>) {
  if ($line =~ /.section/) { next LOOP; }
  if ($line =~ /.previous/) { next LOOP; }
  if ($line =~ /.abicalls/) { next LOOP; }
  if ($line =~ /.type/) { next LOOP; }
  if ($line =~ /.cpload/) { next LOOP; }
  if ($line =~ /.cprestore/) { next LOOP; }
  if ($line =~ /.comm/) { next LOOP; }
  if ($line =~ /.ident/) { next LOOP; }
  if ($line =~ /.size/) { next LOOP; }
  print OUTPUT $line;
} # End while
