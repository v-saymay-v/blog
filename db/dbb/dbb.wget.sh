#!/bin/sh

wget -r --no-parent -R jpg,jpeg,gif,png,css,js -O dbbhelp.log -o dbbhelp.ret http://help.d-bb.com/index.html
nkf -e < dbbhelp.log | removetag | chasen > dbbhelp.txt
grep 'Ì¾»ì' < dbbhelp.txt | db/dbb/chasen2mysql.pl £Ä£Â£Â
grep 'Ì¤ÃÎ¸ì' < dbbhelp.txt | db/dbb/chasen2mysql.pl £Ä£Â£Â

wget -r --no-parent -R jpg,jpeg,gif,png,css,js -O dbbrule.log -o dbbrule.ret http://help.d-bb.com/rulebook2007/index.html
nkf -e < dbbrule.log | removetag | chasen > dbbrule.txt
grep 'Ì¤ÃÎ¸ì' < dbbrule.txt | db/dbb/chasen2mysql.pl £Ä£Â£Â
grep 'Ì¾»ì' < dbbrule.txt | db/dbb/chasen2mysql.pl £Ä£Â£Â

db/dbb/chasen2mysql.pl °ìÈÌ < baseball.txt
