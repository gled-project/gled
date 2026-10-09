# Copyright (C) Matevz Tadel.
# This file is part of Gled.
# SPDX-License-Identifier: LGPL-3.0-or-later

package Gled_Records;

# Reads record files such as the libset manifests. read_file(<file>) returns
# one hash-ref per top-level record, with '_type' set to its struct name.

use Carp;

sub tokenize
{
  my ($text, $file) = @_;
  my @t;
  pos($text) = 0;
  while (1)
  {
    $text =~ m/\G(?:\s+|#[^\n]*)*/gc;
    last if pos($text) >= length($text);
    if    ($text =~ m/\G"((?:[^"\\]|\\.)*)"/gc)
    {
      (my $s = $1) =~ s/\\(.)/$1/g;
      push @t, ['str', $s];
    }
    elsif ($text =~ m/\G([(){}\[\],=])/gc)       { push @t, [$1, $1]; }
    elsif ($text =~ m/\G([^\s(){}\[\],="#]+)/gc) { push @t, ['atom', $1]; }
    else
    {
      croak "$file: bad input at '" . substr($text, pos($text), 20) . "'";
    }
  }
  return \@t;
}

sub new
{
  my ($class, $tokens, $file) = @_;
  return bless { t => $tokens, i => 0, structs => {}, file => $file }, $class;
}

sub peek
{
  my ($p, $off) = @_;
  my $tok = $p->{t}[$p->{i} + ($off || 0)];
  return defined $tok ? $tok->[0] : '';
}

sub take
{
  my ($p, $kind) = @_;
  my $tok = $p->{t}[$p->{i}];
  croak "$p->{file}: unexpected end, expected '$kind'" unless defined $tok;
  croak "$p->{file}: expected '$kind', got '$tok->[1]'" unless $tok->[0] eq $kind;
  ++$p->{i};
  return $tok->[1];
}

sub seq
{
  # Comma-separated items up to the closing token; a trailing comma is fine.
  my ($p, $close, $item) = @_;
  my @out;
  while ($p->peek() ne $close)
  {
    croak "$p->{file}: unexpected end, expected '$close'" if $p->peek() eq '';
    push @out, $item->();
    $p->take(',') if $p->peek() eq ',';
  }
  $p->take($close);
  return @out;
}

sub file
{
  my $p = shift;
  my @recs;
  while ($p->peek() ne '')
  {
    if ($p->peek() eq 'atom' and $p->{t}[$p->{i}][1] eq 'struct' and $p->peek(2) eq '{')
    {
      $p->take('atom');
      my $name = $p->take('atom');
      $p->take('{');
      my @fields = $p->seq('}', sub { $p->take('atom') });
      $p->{structs}{$name} = \@fields;
    }
    else
    {
      push @recs, $p->value();
    }
  }
  return @recs;
}

sub pair
{
  # key=value, blessed so that a record can tell it from a list value.
  my $p = shift;
  my $k = $p->take('atom');
  $p->take('=');
  return bless [$k, $p->value()], 'Gled_Records::KV';
}

sub arg
{
  my $p = shift;
  return $p->pair() if $p->peek() eq 'atom' and $p->peek(1) eq '=';
  return $p->value();
}

sub value
{
  my $p = shift;
  my $k = $p->peek();
  if ($k eq '[')
  {
    $p->take('[');
    return [ $p->seq(']', sub { $p->value() }) ];
  }
  if ($k eq '{')
  {
    $p->take('{');
    my %h;
    for my $kv ($p->seq('}', sub { $p->pair() }))
    {
      croak "$p->{file}: duplicate key '$kv->[0]'" if exists $h{$kv->[0]};
      $h{$kv->[0]} = $kv->[1];
    }
    return \%h;
  }
  return $p->take('str') if $k eq 'str';

  my $v = $p->take('atom');
  return $v unless $p->peek() eq '(';

  $p->take('(');
  my @args   = $p->seq(')', sub { $p->arg() });
  my $fields = $p->{structs}{$v};
  croak "$p->{file}: record '$v' has no struct" unless defined $fields;

  my (@pos, %named);
  for my $a (@args)
  {
    if (ref $a eq 'Gled_Records::KV')
    {
      croak "$p->{file}: $v: duplicate key '$a->[0]'" if exists $named{$a->[0]};
      $named{$a->[0]} = $a->[1];
    }
    else
    {
      push @pos, $a;
    }
  }

  my %rec = ('_type' => $v);
  my $i   = 0;
  for my $f (@$fields)
  {
    if    ($f =~ m/^\@(\w+)$/) { $rec{$1} = [ @pos[$i .. $#pos] ]; $i = @pos; }
    elsif ($f =~ m/^%(\w+)$/)  { $rec{$1} = { %named }; %named = (); }
    else
    {
      croak "$p->{file}: $v: missing '$f'" if $i > $#pos;
      $rec{$f} = $pos[$i++];
    }
  }
  croak "$p->{file}: $v: " . (@pos - $i) . " extra value(s)" if $i < @pos;
  croak "$p->{file}: $v: unknown key(s) " . join(', ', sort keys %named) if %named;
  return \%rec;
}

sub read_file
{
  my $file = shift;
  open(my $fh, '<', $file) or croak "can't open $file";
  my $text = do { local $/; <$fh> };
  close $fh;
  my $p = Gled_Records->new(tokenize($text, $file), $file);
  return $p->file();
}

1;
