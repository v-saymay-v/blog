#!/bin/sh

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O mlb.log -o mlb.ret http://mlb.fc2web.com/mlb30teams.htm
#nkf -e < mlb.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > mlb.ntg
#chasen < mlb.ntg > mlb.txt
grep 'Ì¾»ì' < mlb.txt | ./chasen2mysql.pl £Í£Ì£Â
grep 'Ì¤ÃÎ¸ì' < mlb.txt | ./chasen2mysql.pl £Í£Ì£Â
