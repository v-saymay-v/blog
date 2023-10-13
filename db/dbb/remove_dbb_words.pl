#!/usr/local/bin/perl

use strict;
use DBI;

my $dbhost = 'localhost';
my $dbname = 'dbb_blog';
my $dbuser = 'baseball';
my $dbpass = '6C2DF51772';

&main();
exit 0;

sub main()
{
	my $sth;
	my $db = DBI->connect("DBI:mysql:$dbname:$dbhost", $dbuser, $dbpass) or die "\n\nCan not connect to DB server.";
	while (<STDIN>) {
		if ($_ =~ /\(品詞 \(.* .*\)\) \(\(見出し語 \((.*) .*\)\) \(読み .*\) \(発音 .*\) \)/ ||
		    $_ =~ /\(品詞 \(.*\)\) \(\(見出し語 \((.*) .*\)\) \(読み .*\) \(発音 .*\) \)/) {
			my $t1 = $db->quote($1);
			my $t2 = $t1;
			my $t3 = substr($t1, 1);
			my $t4 = substr($t1, 1);
			chop($t2);
			$t2 = $t2 . "%'";
			$t3 = "'%" . $t3;
			$t4 = "'%" . $t4;
			chop($t4);
			$t4 = $t4 . "%'";
			if ($ARGV[0] && $ARGV[0] eq "-n") {
				$sth = $db->prepare("select count(*) from dt_keywords where c_keyword=$t1 or c_keyword like $t2 or c_keyword like $t3 or c_keyword like $t4;");
			} else {
				$sth = $db->prepare("select count(*) from dt_keywords where c_keyword=$t1;");
			}
			$sth->execute;
			if ($sth->rows) {
				my @value = $sth->fetchrow_array;
				if (int($value[0]) > 0) {
					$sth->finish;
					next;
				}
			}
			$sth->finish;
			print "$_";
		}
	}
}
