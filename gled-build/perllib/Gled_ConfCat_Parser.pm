#!/usr/bin/perl -w

# Copyright (C) Matevz Tadel.
# This file is part of Gled.
# SPDX-License-Identifier: LGPL-3.0-or-later

package Gled_ConfCat_Parser;

use Carp;
use Cwd qw(getcwd);
use File::Basename qw(basename);

$CFGFILE = "$ENV{GLEDSYS}/build_config";

sub import_build_config {
  # imports $GLEDSYS/build_config
  # exports the two hash-refs as $config and $resolver into main::
  croak "config file $CFGFILE not found (configedp?)"
    unless -e "$CFGFILE";
  do "$CFGFILE" or 
    croak"error evaling $CFGFILE";
  $main::config = $VAR1; $main::resolver = $VAR2;
}

sub parse_catalog {
  # Reads the glass list of a libset into the global $CATALOG:
  #   LibSetName, LibID, ClassList (in glass.list order), ClassID2Name and
  #   Classes{<Glass>} with ClassID, Stem and RnrClass.
  # Expects cwd to be the libset directory; the optional argument is the
  # path of the libset directory relative to cwd.
  # The libset name is the name of its directory. The libset ID comes from
  # build_config, where configure puts LIB_SET_ID of the libset Makefile.
  my $dir  = shift;
  my $name = basename(defined $dir ? $dir : getcwd());
  my $list = (defined $dir ? "$dir/" : "") . $main::config->{GLASS_LIST};

  my $lid = $main::resolver->{LibName2LibSpecs}{$name}{LibID};
  croak "no LibID for libset '$name' in build_config" unless defined $lid;

  my $cat = { 'LibSetName' => $name, 'LibID' => $lid,
              'ClassList' => [], 'ClassID2Name' => {}, 'Classes' => {} };

  open(my $fh, '<', $list) or croak "can't open $list";
  while (<$fh>)
  {
    # Columns: class, class id, stem, renderer class ('.' for the class
    # itself, absent for none).
    next if /^\s*(#|$)/;
    my ($c, $id, $stem, $rnr) = split;
    $rnr = "" unless defined $rnr;
    $rnr = $c if $rnr eq ".";
    $cat->{Classes}{$c} = { 'ClassID' => $id, 'Stem' => $stem, 'RnrClass' => $rnr };
    $cat->{ClassID2Name}{$id} = $c;
    push @{$cat->{ClassList}}, $c;
  }
  close $fh;

  $main::CATALOG = $cat;
}

1;
