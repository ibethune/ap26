#!/usr/bin/perl
# Convert OpenCL code to a C-style string.
# I, the author, Ken Brazier, place this Perl code in the Public Domain.
use strict;

my $args = @ARGV;

if ($args != 1) {
	print STDERR "Number of args was $args, usage: cltoh.pl <filename.cl>\n";
	exit;
}

# Remove path and extension: filename is varname
my $varname = @ARGV[0];
$varname =~ s{.*/}{};
$varname =~ s{\.[^.]+$}{};
$varname = $varname."_cl";
my $guard = uc $varname;

# Load the code and start the string!
print <<EOF;
#ifndef $guard
#define $guard

static const char* $varname= \\
EOF

while(<>) {
	# Chomp.
	s/[\r\n]*$//;

	# Fix \'s not at the ends of lines.
	s/\\(.)/\\\\$1/g;

	# Escape quotes - vital!
	s/"/\\"/g;

	# Print the line in a string. Adjacent strings are concatenated.
	if(s/\\$//) {
		# If string ended in an escaped newline, just print the string without a newline.
		print '"', $_, "\" \\\n";
	} else {
		print '"', $_, "\\n\" \\\n";
	}
}
print ";\n#endif\n";
