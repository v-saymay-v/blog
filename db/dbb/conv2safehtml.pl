#!/usr/local/bin/perl5.8.8

use strict;
use HTML::TagFilter;
use DBI;

my $user = 'baseball';
my $passwd = '6C2DF51772';

&main();
exit 0;

sub main()
{
	my @tags = (
		"abbr","acronym","address","area",
		"b","bdo","big","blink","blockquote","br",
		"caption","center","cite","code","col","colgroup","comment",
		"dd","del","dfn","dir","dl","dt","em","fieldset","font",
		"h1","h2","h3","h4","h5","h6","hr",
		"i","ins","isindex","kbd","keygen",
		"label","legend","li","listing",
		"map","marquee","menu","multicol",
		"nextid","nobr","noembed","noframes","nolayer","noscript",
		"ol","optgroup","p","plaintext","pre","q",
		"rb","rp","rt","ruby",
		"s","samp","small","spacer","span","strike","strong","sub","sup",
		"table","tbody","td","tfoot","th","thead","tr","tt","u","ul",
		"var","wbr","xmp");
	my @params = (
		"background", "href", "class", "src", "style",
		"onclick", "ondblclick", "onkeydown", "onkeypress", "onkeyup",
		"onmousedown", "onmouseup", "onmouseover", "onmouseout", "onmousemove",
		"onload", "onunload", "onfocus", "onblur", "onsubmit", "onreset",
		"onchange", "onresize", "onmove", "ondragdrop", "onabort", "onerror", "onselect"
	);
	my $i;
	my $t;
	my $db;
	my $sth;
	my $sth2;
	my $str_sql;
	my $dirty_html;
	my $blog_id;
	my $entry_id;
	my $num_rows;

	my $tf = HTML::TagFilter->new(
		log_rejects => 1,
		strip_comments => 1,
		echo => 0,
		verbose => 0,
	);
	$tf->clear_rules();
	foreach $t (@tags) {
		$tf->allow_tags({$t => {any => []}});
		foreach $i (@params) {
			$tf->deny_tags({$t => {$i => []}});
		}
	}

	$db = DBI->connect('DBI:mysql:dbb_blog:localhost', $user, $passwd) or die "Can not connect to DB server.";
	$sth = $db->prepare("select n_blog_id,n_entry_id,c_entry_body from at_entry");
	$sth->execute;
	$num_rows = $sth->rows;
	for ($i=0; $i < $num_rows; $i++){
		my @value = $sth->fetchrow_array;
    		$blog_id = $value[0];
		$entry_id = $value[1];
		$dirty_html = $value[2];
		$tf->parse($dirty_html);
    		$str_sql ="update at_entry set c_entry_body = '" . $tf->output . "'";
    		$str_sql = $str_sql . " where n_blog_id = " . $blog_id;
		$str_sql = $str_sql . " and n_entry_id = " . $entry_id;
    		$sth2 = $db->prepare($str_sql);
    		$sth2->execute;
    		$sth2->finish;
	}
	$sth->finish;
	$db->disconnect();
}
