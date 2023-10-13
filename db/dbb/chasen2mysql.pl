#!/usr/local/bin/perl

use strict;
use DBI;

my $dbhost = 'localhost';
my $dbname = 'dbb_blog';
my $dbuser = 'root';
my $dbpass = '2F256BDA53';
my $argc;
my $data_type;

$argc = @ARGV;
if ($argc < 1) {
	print "Usage: chasen2mysql type\n";
	exit 1;
}
$data_type = @ARGV[0];

&main();
exit 0;

sub main()
{
	my $db;
	my $sql;
	my $sth;
	my $num_recs;
	my @lines;

	$db = DBI->connect("DBI:mysql:$dbname:$dbhost", $dbuser, $dbpass) or die "Can not connect to DB server.";

	while (<STDIN>) {
		@lines = split(/\t/, $_);
		my $conv = $db->quote($lines[0]);
		$sql = "select count(*) from dt_keywords where c_keyword = $conv;";
		$sth = $db->prepare($sql);
		$sth->execute;
		$num_recs = 0;
		if ($sth->rows) {
			my @value = $sth->fetchrow_array;
			$num_recs = int($value[0]);
		}
		$sth->finish;
		if (!$num_recs) {
			$sql = "insert into dt_keywords (c_keyword,c_kana,c_type) values (";
			$sql = $ sql . $db->quote($lines[0]);
			$sql = $ sql . ",";
			$sql = $ sql . $db->quote($lines[1]);
			$sql = $ sql . ",'" . $data_type. "');";
			$sth = $db->prepare($sql);
			$sth->execute;
			$sth->finish;
		}
	}
}
