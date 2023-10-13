#!/usr/local/bin/perl

use strict;
use CGI;
use DBI;
#use HTML::Entities;

my $dbhost = 'localhost';
my $dbname = 'dbb_blog';
my $dbuser = 'baseball';
my $dbpass = '6C2DF51772';
my @aiueo = ("ア","カ","サ","タ","ナ","ハ","マ","ヤ","ラ","ワ");
my %oeiua = ("ア","オ","カ","コ","サ","ソ","タ","ト","ナ","ノ","ハ","ホ","マ","モ","ヤ","ヨ","ラ","ロ","ワ","ヲ");

&main();
exit 0;

sub print_option
{
	my $i;
	my $dbres;
	$dbres = $_[0]->prepare("select n_kind,c_name from sy_point_kind where n_kind <= 5 order by n_kind");
	$dbres->execute;
	for ($i = 0; $i < $dbres->rows; ++$i) {
		my @value = $dbres->fetchrow_array;
		print "<option value=\"$value[0]\"";
		if ($value[0] eq "$_[1]") {
			print " selected";
		}
		print ">$value[1]</option>\n";
	}
	$dbres->finish;
}

sub print_boxes
{
	my $i;
	my $dbres;
	$dbres = $_[0]->prepare("select n_kind,c_name,n_max from sy_point_kind order by n_kind");
	$dbres->execute;
	for ($i = 0; $i < $dbres->rows; ++$i) {
		my @value = $dbres->fetchrow_array;
		print "<tr><td>$value[1]</td><td><input type=\"text\" name=\"max$value[0]\" value=\"$value[2]\"></tr>\n";
	}
	$dbres->finish;
}

sub main()
{
	my $i;
	my $j;
	my $sth;
	my $sth2;
	my $pgs = 21;
	my $pgshlf = 10;
	my $lns = 30;

	my $q = new CGI;
	my $db = DBI->connect("DBI:mysql:$dbname:$dbhost", $dbuser, $dbpass) or die "\n\nCan not connect to DB server.\n";

	my $del = $q->param('del');
	my $add = $q->param('add');
	my $upd = $q->param('upd');
	my $test = $q->param('test');
	my $dotest = $q->param('dotest');
	my $setmax = $q->param('setmax');
	my $dosetmax = $q->param('dosetmax');
	my $kind = $q->param('kind');
	my $index = $q->param('index');
	my $string = $q->param('string');
	my $search = $q->param('search');
	my $chasen = $q->param('chasen');
	my $start = $q->param('start');
	if (!$start) {
		$start = "0";
	}
	my $page = $q->param('page');
	if (!$page) {
		$page = "0";
	}

	if ($chasen) {
		open(OUT, "> /var/tmp/dbbdic.dic");
		$sth = $db->prepare("select T1.c_keyword,T1.c_kana,T2.c_name,T1.n_point from dt_keywords T1,sy_point_kind T2 where T1.n_kind = T2.n_kind order by T1.c_kana;");
		$sth->execute;
		for ($i = 0; $i < $sth->rows; ++$i) {
			my @value = $sth->fetchrow_array;
			chomp($value[0]);
			chomp($value[1]);
			chomp($value[2]);
			chomp($value[3]);
			if (int($value[3]) < 0) {
				$value[3] = int($value[3]) * (-1);
				$value[3] = "M" . $value[3];
			}
			print OUT "(品詞 (名詞 野球 $value[2] $value[3])) ((見出し語 ($value[0] 0)) (読み $value[1]) (発音 $value[1]))\n";
		}
		$sth->finish;
		close(OUT);
		system("/usr/local/libexec/chasen/makedbbdic /var/tmp/dbbdic.dic");
		$index =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		$string =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		print $q->redirect("edit_dic.cgi?start=$start&page=$page&kind=$kind&index=$index&string=$string");
		unlink("/var/tmp/dbbdic.dic");
	} elsif ($test) {
		$index =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		$string =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		print $q->header(-charset=>'euc-jp');
		print <<END_OF_TEST1;
<html>
<head>
<title>DBB辞書の動作確認</title>
<meta http-equiv="Content-Type" content="text/html; charset=EUC-JP" />
<html>
<body>
<form method="post" action="./edit_dic.cgi">
<input type="hidden" name="start" value="$start">
<input type="hidden" name="page" value="$page">
<input type="hidden" name="index" value="$index">
<input type="hidden" name="string" value="$string">
<h2>DBB辞書の動作確認</h2><br>
テストする書き込みを下記に記入し、「テスト開始」を押してください。<br>
<textarea name="teststr" rows="50" cols="80"></textarea><br>
<input type="submit" name="dotest" value="テスト開始">　
<input type="button" value="テスト中止" onclick="location.href='edit_dic.cgi?start=$start&page=$page&kind=$kind&index=$index&string=$string'">
</form>
END_OF_TEST1
		print $q->end_html;
	} elsif ($dotest) {
		my $teststr = $q->param('teststr');
		$index =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		$string =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		print $q->header(-charset=>'euc-jp');
		print <<END_OF_TEST2;
<html>
<head>
<title>DBB辞書の動作確認</title>
<meta http-equiv="Content-Type" content="text/html; charset=EUC-JP" />
<html>
<body>
<h2>DBB辞書の動作確認</h2><br>
<table border="1" cellspaing=0 cellpadding="2"><tr><td><pre>
END_OF_TEST2
		open(OUT, "> /var/tmp/edit_dic.tmp");
		print OUT "$teststr";
		close(OUT);
		open(IN, "/usr/local/bin/chasen < /var/tmp/edit_dic.tmp | /usr/local/bin/calcdbb |");
		while (<IN>) {
			print $_;
		}
		close(IN);
		unlink("/var/tmp/edit_dic.tmp");
		print "</pre></td></tr></table>\n";
		print "<input type=\"button\" value=\"トップに戻る\" onclick=\"location.href='edit_dic.cgi?start=$start&page=$page&kind=$kind&index=$index&string=$string'\">\n";
		print $q->end_html;
	} elsif ($setmax) {
		$index =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		$string =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		print $q->header(-charset=>'euc-jp');
		print <<END_OF_MAX1;
<html>
<head>
<title>得点最大値の登録</title>
<meta http-equiv="Content-Type" content="text/html; charset=EUC-JP" />
<html>
<body>
<form method="post" action="./edit_dic.cgi">
<input type="hidden" name="start" value="$start">
<input type="hidden" name="page" value="$page">
<input type="hidden" name="index" value="$index">
<input type="hidden" name="string" value="$string">
<h2>得点最大値の登録</h2><br>
各タイプの最大得点を入力し、「 登　録 」を押してください。<br>
最大得点を設定しない場合は「0」を入力してください。<br>
<br>
<table border="1" cellspacing="0" cellpadding="2">
<tr><td>タイプ</td><td>得点</td></tr>
END_OF_MAX1
		&print_boxes($db);
		print <<END_OF_MAX2;
</table>
<br>
「ＢＬ」および「ＣＡ」は最大得点ではなく、得点からマイナスする数値です。
<br>
<br>
<input type="submit" name="dosetmax" value=" 登　録 ">　
<input type="button" value=" 中　止 " onclick="location.href='edit_dic.cgi?start=$start&page=$page&kind=$kind&index=$index&string=$string'">
</form>
END_OF_MAX2
		print $q->end_html;
	} elsif ($dosetmax) {
		$sth = $db->prepare("select n_kind from sy_point_kind where n_kind <= 5 order by n_kind");
		$sth->execute;
		for ($i = 0; $i < $sth->rows; ++$i) {
			my @value = $sth->fetchrow_array;
			my $max = $q->param("max$value[0]");
			if ($max) {
				$sth2 = $db->prepare("update sy_point_kind set n_max = $max where n_kind = $value[0];");
			} else {
				$sth2 = $db->prepare("update sy_point_kind set n_max = 0 where n_kind = $value[0];");
			}
			$sth2->execute;
			$sth2->finish;
		}
		my $word = $q->param('word');
		my $kana = $q->param('kana');
		my $type = $q->param('type');
		my $point = $q->param('point');
		$index =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		$string =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		print $q->redirect("edit_dic.cgi?start=$start&page=$page&kind=$kind&index=$index&string=$string");
	} elsif ($add) {
		my $word = $q->param('word');
		my $kana = $q->param('kana');
		my $type = $q->param('type');
		my $point = $q->param('point');
		$sth = $db->prepare("insert into dt_keywords (c_keyword,c_kana,n_kind,n_point) values ('$word','$kana','$type', $point);");
		$sth->execute;
		$sth->finish;
		$index =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		$string =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		print $q->redirect("edit_dic.cgi?start=$start&page=$page&kind=$kind&index=$index&string=$string");
	} elsif ($upd) {
		my $max = $q->param('max');
		for ($i = 0; $i <= $max; ++$i) {
			my $word = $q->param("word_$i");
			if ($word) {
				my $kana = $q->param("kana_$i");
				my $type = $q->param("type_$i");
				my $point = $q->param("point_$i");
				$sth = $db->prepare("update dt_keywords set c_kana = '$kana', n_kind = '$type', n_point = $point where n_seq = $word;");
				$sth->execute;
				$sth->finish;
			}
		}
		$index =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		$string =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		print $q->redirect("edit_dic.cgi?start=$start&page=$page&kind=$kind&index=$index&string=$string");
	} elsif ($del) {
		my $max = $q->param('max');
		for ($i = 0; $i <= $max; ++$i) {
			my $word = $q->param("word_$i");
			if ($word) {
				$sth = $db->prepare("delete from dt_keywords where n_seq = $word;");
				$sth->execute;
				$sth->finish;
			}
		}
		$index =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		$string =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		print $q->redirect("edit_dic.cgi?start=$start&page=$page&kind=$kind&index=$index&string=$string");
	} else {
		my $ps;
		my $sql;
		my $where;
		my $prev;
		my $next;
		my $newpage;
		my $newstart;

		if ($search) {
			$start = "0";
			$page = "0";
		}
		$where = '';
		$sql = "select sql_calc_found_rows T1.c_keyword,T1.c_kana,T1.n_kind,T1.n_point,T1.n_seq from dt_keywords T1";
		if (($kind && $kind ne '') || ($index && $index ne '') || ($string && $string ne '')) {
			if ($kind ne '') {
				$where = " where n_kind='$kind'"
			}
			if ($index ne '') {
				if ($where eq '') {
					$where = " where";
				} else {
					$where = $where . " and";
				}
				$where = $where . " (substring(c_kana, 1, 1) >= '$index' and substring(c_kana, 1, 1) <= '$oeiua{$index}')";
			}
			if ($string ne '') {
				if ($where eq '') {
					$where = " where";
				} else {
					$where = $where . " and";
				}
				$where = $where . " (c_keyword = '$string' or c_keyword like '%$string' or c_keyword like '$string%' or c_keyword like '%$string%')"
			}
		}
		$sql = $sql . $where . " order by c_kana,c_keyword,n_seq limit $start,$lns;";

		$sth = $db->prepare($sql);
		$sth->execute;
		$sth2 = $db->prepare("select found_rows()");
		$sth2->execute;
		my @value2 = $sth2->fetchrow_array;

		print $q->header(-charset=>'euc-jp');
		print <<END_OF_DATA1;
<html>
<head>
<title>DBB辞書編集</title>
<meta http-equiv="Content-Type" content="text/html; charset=EUC-JP" />
<script language="JavaScript" type="text/javascript">
<!--
function check_input()
{
	var err = '';
	var val;
	val = document.forms['list_word'].elements['word'].value;
	if (!val || val == '') {
		err = '「キーワード」が入力されていません。\\r\\n';
	}
	val = document.forms['list_word'].elements['kana'].value;
	if (!val || val == '') {
		err = err + '「読み」が入力されていません。\\r\\n';
	}
	val = document.forms['list_word'].elements['type'].value;
	if (!val || val == '') {
		err = err + '「種別」が選択されていません。\\r\\n';
	}
	val = document.forms['list_word'].elements['point'].value;
	if (!val || val == '') {
		err = err + '「得点」が選択されていません。\\r\\n';
	}
	if (err != '') {
		alert(err);
		return false;
	}
	return true;
}

function check_overwrite(url)
{
	if (!confirm('茶筌の辞書ファイルを上書きします、よろしいですか？')) {
		return false;
	}
	location.href = url;
	return true;
}

function CheckAll()
{
	var dml = document.list_word;
	var len = dml.elements.length;
	var i;
	for ( i = 0; i < len; i++) {
		if (dml.elements[i].type == 'checkbox' && dml.elements[i].name.indexOf("word_",0) != -1) {
			dml.elements[i].checked = true;
		}
	}
}
//-->
</script>
</head>
<body>
<h2>DBB辞書の編集</h2><br>
<form name="list_word" method="post" action="edit_dic.cgi">
<input type="hidden" name="start" value="$start">
<input type="hidden" name="page" value="$page">
<input type="hidden" name="max" value="$lns">
<table border="1" cellspacing="0" cellpadding="2">
<tr>
	<td colspan="6">
		<table border="0" width="100%">
		<tr>
			<td width="25%" nowrap>
				タイプ
				<select name="kind">
END_OF_DATA1

		print "<option value=\"\"";
		if (!$kind || $kind eq '') {
			print " selected";
		}
		print ">全部</option>\n";

		&print_option($db, $kind);

		print <<END_OF_DATA11;
				</select>
			</td>
			<td width="25%" nowrap>
				行
				<select name="index">
END_OF_DATA11

		print "<option value=\"\"";
		if (!$index || $index eq '') {
			print " selected";
		}
		print ">全部</option>\n";

		foreach $i (@aiueo) {
			print "<option value=\"$i\"";
			if ($index eq $i) {
				print " selected";
			}
			print ">$i行</option>\n";
		}

		print <<END_OF_DATA12;
				</select>
			</td>
			<td width="50%" nowrap>
				<input type="text" name="string" value="$string" style=\"width: 75%\">
				<input type="submit" name = "search" value="検索">
			</td>
		</tr>
		</table>
	</td>
</tr>
<tr>
	<td colspan="6">
		<table border="0" width="100%">
		<tr>
			<td>
END_OF_DATA12

		$index =~ s/(.)/sprintf '%%%x', ord($1)/eg;
		$string =~ s/(.)/sprintf '%%%x', ord($1)/eg;

		if ($value2[0] / $lns > $pgs && $start / $lns > $pgshlf) {
			$ps = ($start / $lns) - $pgshlf;
		} else {
			$ps = 0;
		}
		if (!$page) {
			print "←　";
		} else {
			$prev = $start - $lns;
			print "<a href=\"edit_dic.cgi?start=$prev&page=$ps&kind=$kind&index=$index&string=$string\">←</a>　";
		}
		print "</td>\n";

		print "<td align=\"center\">\n";
		$ps = $page;
		while ($ps < $page + $pgs && $ps < $value2[0] / $lns) {
			$newstart = $ps * $lns;
			if ($value2[0] / $lns > $pgs && $ps > $pgshlf) {
				$newpage = $ps - $pgshlf;
			} else {
				$newpage = 0;
			}
			++$ps;
			if ($start / $lns == $ps - 1) {
				print "<b>[$ps]</b>\n";
			} else {
				print "<a href=\"edit_dic.cgi?start=$newstart&page=$newpage&kind=$kind&index=$index&string=$string\">$ps</a>\n";
			}
		}
		print "</td>\n";

		print "<td align=\"right\">";
		if ($ps >= $value2[0] / $lns) {
			print "　→";
		} else {
			$next = $start + $lns;
			if ($next / $lns > $pgshlf) {
				$ps = $next / $lns - $pgshlf;
			} else {
				$ps = 0;
			}
			print "　<a href=\"edit_dic.cgi?start=$next&page=$ps&kind=$kind&index=$index&string=$string\">→</a>";
		}
		print "</td>\n";

		$i = $sth->rows;
		print <<END_OF_DATA21;
		</tr>
		</table>
	</td>
</tr>
<tr>
	<th>No.</th><th>キーワード</th><th>読み</th><th>タイプ</th><th>得点</th><th><a href="#" onclick="CheckAll()">選択</a></th>
</tr>
<tr>
	<td><br></td>
	<td><input type="text" name="word" style="width: 95%"></td>
	<td><input type="text" name="kana" style="width: 95%"></td>
	<td align="center"><select name="type">
		<option value="">−</option>
END_OF_DATA21

		&print_option($db, "0");

	print <<END_OF_DATA22;
	</select></td>
	<td align="center"><select name="point">
		<option value="-10">-10</option>
		<option value="-9">-9</option>
		<option value="-8">-8</option>
		<option value="-7">-7</option>
		<option value="-6">-6</option>
		<option value="-5">-5</option>
		<option value="-4">-4</option>
		<option value="-3">-3</option>
		<option value="-2">-2</option>
		<option value="-1">-1</option>
		<option value="" selected>−</option>
		<option value="1">1</option>
		<option value="2">2</option>
		<option value="3">3</option>
		<option value="4">4</option>
		<option value="5">5</option>
		<option value="6">6</option>
		<option value="7">7</option>
		<option value="8">8</option>
		<option value="9">9</option>
		<option value="10">10</option>
	</select></td>
	<td align="center"><input type="submit" name="add" value="追加" onclick="return check_input()"></td>
</tr>
END_OF_DATA22

		for ($i = 0; $i < $sth->rows; ++$i) {
			my @value = $sth->fetchrow_array;
			my $enc = $value[0];
			print "<tr>";
			print "<td>$value[4]</td>";
			print "<td>$enc</td>";
			print "<td><input type=\"text\" name=\"kana_$i\" value=\"$value[1]\" style=\"width: 95%\"></td>";
			print "<td align=\"center\">\n";
			print "<select name=\"type_$i\">\n";
			print "<option value=\"\">−</option>\n";
			&print_option($db, $value[2]);
			print "</select></td>\n";
			print "<td align=\"center\"><select name=\"point_$i\">\n";
			for ($j = -10; $j <= 10; $j += 1) {
				print "<option value=\"$j\"";
				if ($value[3] == $j) {
					print " selected";
				}
				print ">$j</option>\n";
			}
			print "</select></td>\n";
			print "<td align=\"center\"><input type=\"checkbox\" name=\"word_$i\" value=\"$value[4]\"></td>\n";
			print "</tr>\n";
		}
		$sth->finish;
		$sth2->finish;

		print <<END_OF_DATA3;
<tr>
	<td colspan="5">
		<input type="button" value="茶筌の辞書ファイルに同期" onclick="return check_overwrite('edit_dic.cgi?start=$start&page=$page&chasen=1&kind=$kind&index=$index&string=$string')">
		<input type="button" value="動作の確認" onclick="location.href='edit_dic.cgi?start=$start&page=$page&test=1&kind=$kind&index=$index&string=$string'">
		<input type="button" value="最大点数の設定" onclick="location.href='edit_dic.cgi?start=$start&page=$page&setmax=1&kind=$kind&index=$index&string=$string'">
	</td>
	<td align="center">
		<input type="submit" name="upd" value="更新">
		<input type="submit" name="del" value="削除" onclick="return confirm('選択された名詞をすべて削除します、よろしいですか？')">
	</td>
</tr>
</table>
</form>
END_OF_DATA3

		print $q->end_html;
	}
}
