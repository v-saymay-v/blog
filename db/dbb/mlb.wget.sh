#!/bin/sh

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O mlb.log -o mlb.ret http://mlb.fc2web.com/mlb30teams.htm
#nkf -e < mlb.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/��//g' > mlb.ntg
#chasen < mlb.ntg > mlb.txt
grep '̾��' < mlb.txt | ./chasen2mysql.pl �̣ͣ�
grep '̤�θ�' < mlb.txt | ./chasen2mysql.pl �̣ͣ�
