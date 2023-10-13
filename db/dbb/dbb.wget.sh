#!/bin/sh

wget -r --no-parent -R jpg,jpeg,gif,png,css,js -O dbbhelp.log -o dbbhelp.ret http://help.d-bb.com/index.html
nkf -e < dbbhelp.log | removetag | chasen > dbbhelp.txt
grep '̾��' < dbbhelp.txt | db/dbb/chasen2mysql.pl �ģ£�
grep '̤�θ�' < dbbhelp.txt | db/dbb/chasen2mysql.pl �ģ£�

wget -r --no-parent -R jpg,jpeg,gif,png,css,js -O dbbrule.log -o dbbrule.ret http://help.d-bb.com/rulebook2007/index.html
nkf -e < dbbrule.log | removetag | chasen > dbbrule.txt
grep '̤�θ�' < dbbrule.txt | db/dbb/chasen2mysql.pl �ģ£�
grep '̾��' < dbbrule.txt | db/dbb/chasen2mysql.pl �ģ£�

db/dbb/chasen2mysql.pl ���� < baseball.txt
