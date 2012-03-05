#!/usr/bin/perl -w

# Based on Preprocessor written by Philipp Koehn
# Changed only to detect document separator and skip running on those lines

binmode(STDIN, ":encoding(UTF-8)");
binmode(STDOUT, ":encoding(UTF-8)");
binmode(STDERR, ":encoding(UTF-8)");

use FindBin qw($Bin);
use strict;

my %NONBREAKING_PREFIX = ();
my $dir;
my $HELP = 0;
my $language = "en";

while (@ARGV) {
	$_ = shift;
	/^-l$/ && ($language = shift, next);
	/^-d$/ && ($dir = shift, next);
	/^-h$/ && ($HELP = 1, next);
}

if ($HELP) {
    print "Usage ./split-sentences.perl -d data_dir -l language < textfile > splitfile\n";
	exit;
}

die ("ERROR: No splitcode.h found in $dir\n") unless (-e "$dir/splitcode.h");
my $splitcode = "";
open FILE, "$dir/splitcode.h";
my $line;
foreach $line (<FILE>) {
    chomp($line);
    if ( $line =~ m/SPLITCODE / ) {
        my @arr = split(/ /, $line);
        $splitcode = $arr[2];
        $splitcode =~ s/^\"//;
        $splitcode =~ s/\"$//;
        last;
    }
}

my $prefixfile = "$dir/nonbreaking_prefix.$language";
die ("ERROR: No abbreviation file at $prefixfile\n") unless (-e $prefixfile);

if (-e "$prefixfile") {
	open(PREFIX, "<:utf8", "$prefixfile");
	while (<PREFIX>) {
		my $item = $_;
		chomp($item);
		if (($item) && (substr($item,0,1) ne "#")) {
			if ($item =~ /(.*)[\s]+(\#NUMERIC_ONLY\#)/) {
				$NONBREAKING_PREFIX{$1} = 2;
			} else {
				$NONBREAKING_PREFIX{$item} = 1;
			}
		}
	}
	close(PREFIX);
}

##loop text, add lines together until we get a blank line or a <p>
my $text = "";
my $header = "";
my $footer = "";
while(<STDIN>) {
	chop;
	if (/^<.+>$/ || /^\s*$/) {
		#time to process this block, we've hit a blank or <p>
		&do_it_for($text,$_);
		print "<P>\n" if (/^\s*$/ && $text); ##if we have text followed by <P>
		$text = "";
	}
    elsif (/^DOCSTART $splitcode/) {
        $header = $_;
        print "\n$header\n";
    }
    elsif (/^DOCEND $splitcode/) {
        $footer = $_;
        print "\n$footer\n";
    }
	else {
		#append the text, with a space
		$text .= $_. " ";
	}
}

#do the leftover text
&do_it_for($text,"") if $text;


sub do_it_for {
	my($text,$markup) = @_;
	print &preprocess($text) if $text;
	print "$markup\n" if ($markup =~ /^<.+>$/);
	#chop($text);
}

sub preprocess {
	# clean up spaces at head and tail of each line as well as any double-spacing
	$text =~ s/ +/ /g;
	$text =~ s/\n /\n/g;
	$text =~ s/ \n/\n/g;
	$text =~ s/^ //g;
    $text =~ s/ $//g;
	
	#this is one paragraph
	my($text) = @_;
	
	#####add sentence breaks as needed#####
	
	#non-period end of sentence markers (?!) followed by sentence starters.
	$text =~ s/([?!]) +([\'\"\(\[\¿\¡\p{IsPi}]*[\p{IsUpper}])/$1\n$2/g;
		
	#multi-dots followed by sentence starters
	$text =~ s/(\.[\.]+) +([\'\"\(\[\¿\¡\p{IsPi}]*[\p{IsUpper}])/$1\n$2/g;
	
	# add breaks for sentences that end with some sort of punctuation inside a quote or parenthetical and are followed by a possible sentence starter punctuation and upper case
	$text =~ s/([?!\.][\ ]*[\'\"\)\]\p{IsPf}]+) +([\'\"\(\[\¿\¡\p{IsPi}]*[\ ]*[\p{IsUpper}])/$1\n$2/g;
		
	# add breaks for sentences that end with some sort of punctuation are followed by a sentence starter punctuation and upper case
	$text =~ s/([?!\.]) +([\'\"\(\[\¿\¡\p{IsPi}]+[\ ]*[\p{IsUpper}])/$1\n$2/g;
	
	# special punctuation cases are covered. Check all remaining periods.
	my $word;
	my $i;
	my @words = split(/ /,$text);
	$text = "";
	for ($i=0;$i<(scalar(@words)-1);$i++) {
		if ($words[$i] =~ /([\p{IsAlnum}\.\-]*)([\'\"\)\]\%\p{IsPf}]*)(\.+)$/) {
			#check if $1 is a known honorific and $2 is empty, never break
			my $prefix = $1;
			my $starting_punct = $2;
			if($prefix && $NONBREAKING_PREFIX{$prefix} && $NONBREAKING_PREFIX{$prefix} == 1 && !$starting_punct) {
				#not breaking;
			} elsif ($words[$i] =~ /(\.)[\p{IsUpper}\-]+(\.+)$/) {
				#not breaking - upper case acronym	
			} elsif($words[$i+1] =~ /^([ ]*[\'\"\(\[\¿\¡\p{IsPi}]*[ ]*[\p{IsUpper}0-9])/) {
				#the next word has a bunch of initial quotes, maybe a space, then either upper case or a number
				$words[$i] = $words[$i]."\n" unless ($prefix && $NONBREAKING_PREFIX{$prefix} && $NONBREAKING_PREFIX{$prefix} == 2 && !$starting_punct && ($words[$i+1] =~ /^[0-9]+/));
				#we always add a return for these unless we have a numeric non-breaker and a number start
			}
			
		}
		$text = $text.$words[$i]." ";
	}
	
	#we stopped one token from the end to allow for easy look-ahead. Append it now.
	$text = $text.$words[$i];
	
	# clean up spaces at head and tail of each line as well as any double-spacing
	$text =~ s/ +/ /g;
	$text =~ s/\n /\n/g;
	$text =~ s/ \n/\n/g;
	$text =~ s/^ //g;
	$text =~ s/ $//g;
	
	#add trailing break
	$text .= "\n" unless $text =~ /\n$/;
	
	return $text;
	
}


