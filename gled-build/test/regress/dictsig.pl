#!/usr/bin/perl
# Summarise what a set of rootcling dictionary sources selects, one line per
# item, sorted, so that two builds can be diffed:
#   class     <name> ver=<version expr> <instance.Set* calls>
#   alt       AddClassAlternate(...) calls
#   hdrname   names in classesHeaders[] (classes, typedefs, functions, globals);
#             empty for dictionaries built as C++ modules
#   namespace namespaces with a GenerateInitInstance()
#
# Usage: dictsig.pl <gled-build>/libsets/*/dict/*.cc > x.sig

use strict;
use warnings;

my %out;
for my $f (@ARGV)
{
  open(my $fh, '<', $f) or die "can't open $f";
  my $src = do { local $/; <$fh> };
  close $fh;

  while ($src =~ m/static ::ROOT::TGenericClassInfo\s*\n?\s*instance\("([^"]+)",\s*([^,]+),(.*?)return &instance;/sg)
  {
    my ($name, $ver, $body) = ($1, $2, $3);
    $ver =~ s/^\s+|\s+$//g;
    my %sets = map { $_ => 1 } $body =~ m/instance\.(Set\w+|AdoptCollection\w*|AdoptStreamer\w*)/g;
    $out{"class $name ver=$ver " . join(' ', sort keys %sets)} = 1;
  }
  while ($src =~ m/(RecordReadRules|AddClassAlternate\("[^"]+","[^"]+"\))/g)
  {
    $out{"alt $1"} = 1;
  }
  if ($src =~ m/static const char\* classesHeaders\[\] = \{(.*?)nullptr\s*\};/s)
  {
    my $h = $1;
    $out{"hdrname $1"} = 1 while $h =~ m/"([^"@]+)", payloadCode, "@"/g;
  }
  while ($src =~ m/TGenericClassInfo \*GenerateInitInstance\(\)\s*\{[^}]*?instance\("([^"]+)"/sg)
  {
    $out{"namespace $1"} = 1;
  }
}
print "$_\n" for sort keys %out;
