#!/usr/local/bin/perl5.8.8

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
	my $i;
	my $db;
	my $sth;
	my $sth2;
	my $str_sql;
	my $body;
	my $blog_id;
	my $entry_id;
	my $num_rows;
	my $name;
	my $value;

	$db = DBI->connect("DBI:mysql:$dbname:$dbhost", $dbuser, $dbpass) or die "Can not connect to DB server.";
	$sth = $db->prepare("select n_blog_id,n_entry_id,c_entry_body from at_entry where n_point is not null");
	$sth->execute;
	$num_rows = $sth->rows;
	for ($i=0; $i < $num_rows; $i++){
		my $tmpfile = "/tmp/tmp.dbb_blog_entry";
		my $point = 0;
		my %temp;
		my %kind;
		my @value = $sth->fetchrow_array;
		$blog_id = $value[0];
		$entry_id = $value[1];
		$body = $value[2];
		open(OUT, "> $tmpfile");
		print(OUT $body);
		close(OUT);
		open(IN, "/usr/local/bin/chasen < $tmpfile | /usr/bin/grep `cat search.txt` |");
		while (<IN>) {
			my @line = split(/\t/, $_);
			my $word = $line[0];
			chomp($line[3]);
			my @item = split(/-/, $line[3]);
			if (!$temp{$line[0]}) {
				$item[3] =~ s/M/-/;
				$temp{$line[0]} = $item[3];
				$point += $item[3];
				if ($kind{$item[2]}) {
					$kind{$item[2]} += $item[3];
				} else {
					$kind{$item[2]} = $item[3];
				}
			}
		}
		close(IN);
		unlink($tmpfile);
		$str_sql ="update at_entry set n_point = " . $point;
		$str_sql = $str_sql . " where n_blog_id = " . $blog_id;
		$str_sql = $str_sql . " and n_entry_id = " . $entry_id;
		$sth2 = $db->prepare($str_sql);
		$sth2->execute;
		$sth2->finish;

		$str_sql = "delete from at_point_detail where n_blog_id = $blog_id and n_entry_id = $entry_id";
		$sth2 = $db->prepare($str_sql);
		$sth2->execute;
		$sth2->finish;
		while (($name, $value) = each(%kind)) {
			my $quote = $db->quote($name);
			$str_sql = "insert into at_point_detail select $blog_id,$entry_id,T1.n_kind,$value from sy_point_kind T1 where T1.c_name = $quote";
			$sth2 = $db->prepare($str_sql);
			$sth2->execute;
			$sth2->finish;
		}
	}
	$sth->finish;


	$db->disconnect();
}
