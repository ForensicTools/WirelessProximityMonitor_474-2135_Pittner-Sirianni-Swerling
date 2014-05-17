#!/usr/bin/perl
# launchChart.pl
# For use with Wireless Proximity Analyzer
# Ross Swerling, Joseph Siriani, Cal Pitner
# Takes JSON, looks up OUI vendor per frame, 
#  tallies strongest signal strength per vendor, outputs JSON file of this.
# v0.1 5/2014
# Dependencies:
#	firefox alias in path (not mac osx)
#	updated oui.txt: this must be stored in the same directory as
#		this script, and can be downloaded from http://standards.ieee.org/develop/regauth/oui/oui.txt

use warnings;
use strict;

my %vendors = (); #hash for storing vendor+strongest signal strength

open(my $INFILE, "< chart/input.csv") || die "cannot open input file";

#process input data into hash
while(<$INFILE>) {
	my @parts = split(/,/, $_);
	my $vendor = ouiLookup($parts[0]);
	chomp($parts[1]);

	if(exists($vendors{$vendor})) {
		if($vendors{$vendor} < $parts[1]) {
			$vendors{$vendor} = $parts[1];
		}
	} 
	else {
		$vendors{$vendor} = $parts[1];
	}
}	
close $INFILE;

open(my $OUTFILE, "> chart/output.json") || die "cannot open output file";

#print hash table to JSON
my $first = 1; #this makes sure there is no leading or trailing comma
print $OUTFILE "[";
foreach( keys( %vendors ) ) {	#may be able to get rid of the sort function. need to test with real data

	if($first) {
		print $OUTFILE "\n[\"$_\", $vendors{$_}]";
		$first = 0;
	}
	else{ print $OUTFILE ",\n[\"$_\", $vendors{$_}]" };
}
print $OUTFILE "\n]";

close $OUTFILE;

`firefox chart/chart.html` || `open chart/chart.html -a Firefox.app`;


## MAC address OUI checker
## Ross Swerling, Joseph Siriani, Cal Pitner
## v0.1 5/2014
## BASED ON:
#   MAC address OUI checker
#   Thijs (Thice) Bosschert
#   http://www.thice.nl
#   v0.4 25-06-2010 
sub ouiLookup {

	# Removing seperators from MAC address
	my $OUI = $_[0];
	$OUI =~ s/[^0-9A-F]//g;

	# Get OUI from MAC
	if ($OUI =~ /^([0-9A-F]{6})/) {
	        $OUI = $1;
	}

	open(my $OUIIN, "<", "chart/oui.txt") || die "  Error: Can not access OUI file";
	while(<$OUIIN>) {
		my $line = $_;
        	my ($checkoui,$company) = split(/\(hex\)/,$line);
	        $checkoui =~ s/[-|\s]//g;
        	# Check if OUI can be found in the list
	        if ($OUI eq $checkoui) {
        	        $company =~ s/\t//g;
                	chomp($company);
	                # Output found OUI
        	        return $company;
        	}
	}
	close($OUIIN);
}
