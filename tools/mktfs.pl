#!/usr/bin/perl


#HEADER**********************************************************************
#
# Copyright 2014 Freescale Semiconductor, Inc.
#
# This software is owned or controlled by Freescale Semiconductor.
# Use of this software is governed by the Freescale MQX RTOS License
# distributed with this Material.
# See the MQX_RTOS_LICENSE file distributed for more details.
#
# Brief License Summary:
# This software is provided in source form for you to use free of charge,
# but it is not open source software. You are allowed to use this software
# but you cannot redistribute it or derivative works of it in source form.
# The software may be used only in connection with a product containing
# a Freescale microprocessor, microcontroller, or digital signal processor.
# See license agreement file for full license terms including other
# restrictions.
#END************************************************************************


# Perl script to generate source code from static web pages - generates static file system.
#
# DIRECTORY STRUCTURE
#  +-data - contain web sources - (html, css, jpg, gif, ...)
#  |   +-index.html
#  |   +-header.jpg
#  |   +-style.css
#  |
#  +-mktfs
# 
# Run script from directory containing data directory! Script recursively searches files 
# in data subdirectory. Script generates file tfs_data.c which contains converted data 
# (pages, pictures, ...) in C constant arrays. Separate C files can be created for selected
# input files.
# 
# ClearCase Perl:
# 	ccperl mktfs.pl -s <separate_file> <input directory>
#
# 	Example: ccperl mktfs.pl -s image.bmp my_web
#
# Perl:
# 	perl mktfs.pl -s <separate_file> <input directory>
#
# 	Example: perl mktfs.pl -s image.bmp my_web


use File::Find;
use File::Compare;


# Get input

%SEPARATE_FILES = ();
$INPUT_DIR = "";
while(@ARGV)
{
  if ($ARGV[0] =~ /^-s$/)
  {
    shift @ARGV;
    if (@ARGV == 0) { last; }
    $tmp = $ARGV[0];
    $tmp =~ s#\\#/#g;
    $SEPARATE_FILES{$tmp} = 1;
  } else {
    $INPUT_DIR = $ARGV[0];
    shift @ARGV;
    last;
  }
  shift @ARGV;
}

# Check input

if (($INPUT_DIR =~ /^$/) || (@ARGV))
{
  my $readme =
      "\nFreescale Semiconductor MQX(TM) MaKe Trivial FileSystem tool\n\n".
      "\tThis tool creates C language source file tfs_data.c with\n".
      "\tconstant arrays of binary data of all input directory files.\n".
      "\tData of selected files can be stored in separate C files.\n";

  print "$readme\n";
  print "Usage:\n";
  print "mktfs.exe [-s <separate_file>] <input_directory>\n";
  exit(0);
}

# Gather files

@INPUT_FILES = ();
find (\&get_files, $INPUT_DIR);

# Open tfs_data.tmp for writing

open(OUTPUT, "> tfs_data.tmp") or die "Can't create temporary file tfs_data.tmp!\n";
print(OUTPUT "#include <tfs.h>\n\n");
print(OUTPUT "extern const TFS_DIR_ENTRY tfs_data[];\n\n");
foreach $file (@INPUT_FILES)
{
  $fvar = "tfs_" . $file;
  $fvar =~ s#[/\.]#_#g;
  if ($SEPARATE_FILES{$file})
  {
    $size = -s $file;
    print(OUTPUT "extern const unsigned char " . $fvar . "[$size];\n\n");
  }
}

# Process input files

foreach $file (@INPUT_FILES)
{
  print "Processing file $file\n";
  $fvar = "tfs_" . $file;
  $fvar =~ s#[/\.]#_#g;
  &process_file ($file, $fvar, $SEPARATE_FILES{$file});
}

# Finish tfs_data.tmp file

print(OUTPUT "const TFS_DIR_ENTRY tfs_data[] = {\n");
foreach $file (@INPUT_FILES)
{
  $fvar = "tfs_" . $file;
  $fvar =~ s#[/\.]#_#g;
  $dest = $file;
  $dest =~ s/^$INPUT_DIR//;
  print(OUTPUT "\t{ \"${dest}\", 0, ");
  print(OUTPUT "(unsigned char*)${fvar}, sizeof(${fvar}) },\n");
}
print(OUTPUT "\t{ 0, 0, 0, 0 }\n};\n\n");
close(OUTPUT);

# Rename temporary to *.c files

$SEPARATE_FILES{"data"} = 1;
@INPUT_FILES = (@INPUT_FILES, "data");
foreach $file (@INPUT_FILES)
{
  $fvar = "tfs_" . $file;
  $fvar =~ s#[/\.]#_#g;
  if ($SEPARATE_FILES{$file})
  {
    if (compare("${fvar}.tmp", "${fvar}.c") == 0)
    {
      unlink("${fvar}.tmp");
    } else {
      if (&check_write_protect("${fvar}.c"))
      {
        rename("${fvar}.tmp", "${fvar}.c");
      } else {
        unlink("${fvar}.tmp");
      }
    }
  }
}
print "Done.\n";
exit(0);


################################################################################


sub get_files
{
  if ((-f $_) && ($File::Find::name !~ /(CVS|~)/))
  {
    $tmp = $File::Find::name;
    $tmp =~ s#^/##;
    @INPUT_FILES = (@INPUT_FILES, $tmp);
  }
}


sub check_write_protect
{
  my ($file) = @_;
  
  if (-e $file && ! -w $file)
  {
    print "Overwriting read only file $file, are you sure? ";
    $response = <STDIN>;
    chomp($response);
    if ($response !~ /[Yy]/)
    {
      return 0;
    }
    system("attrib -R $file");
  }
  return 1;
}


sub process_file
{
  my ($file, $fvar, $separate) = @_;

  open(FILE, $file) or die "Can't open file ${file}!\n";

  binmode(FILE);

  $output = "OUTPUT";
  if ($separate)
  {
    $output = "SEPARATE";
    open($output, "> ${fvar}.tmp") or die "Can't create temporary file ${fvar}.tmp!\n";
    print($output "extern const unsigned char " . $fvar . "[];\n\n");
    print($output "const unsigned char " . $fvar . "[] = {\n");
  } else {
    print($output "static const unsigned char " . $fvar . "[] = {\n");
  }

  print($output "\t/* $file */\n");

  $sep = "\t";
  while(read(FILE, $data, 10, 0))
  {
    foreach(split(//, $data))
    {
      printf($output "${sep}0x%02x", ord($_));
      $sep = ", ";
    }
    $sep = ",\n\t";
  }

  print($output "\n};\n\n");

  if ($separate)
  {
    close($output);
  }
  
  close(FILE);
}

