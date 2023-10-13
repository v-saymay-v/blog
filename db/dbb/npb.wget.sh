#!/bin/sh

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O npb.log -o npb.ret http://www.npb.or.jp/#nkf -e < npb.log | removetag | sed 's/ //g' | sed '/^$/d' | sed 's/¡¡//g' > npb.ntg
#chasen < npb.ntg > npb.txt
#grep 'Ì¾»ì' < npb.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < npb.txt | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O fighters.log -o fighters.ret http://www.fighters.co.jp/
#nkf -e < fighters.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > fighters.ntg
#chasen < fighters.ntg > fighters.txt
#grep 'Ì¾»ì' < fighters.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < fighters.txt | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O lions.log -o lions.ret -U "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)" http://www.seibulions.jp/
#nkf -e < lions.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > lions.ntg
#chasen < lions.ntg > lions.txt
#grep 'Ì¾»ì' < lions.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < lions.txt | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O hawks.log -o hawks.ret http://www.softbankhawks.co.jp/
#nkf -e < hawks.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > hawks.ntg
#chasen < hawks.ntg > hawks.txt
#grep 'Ì¾»ì' < hawks.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < hawks.txt | sed s/¡¡// | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O marines.log -o marines.ret http://www.marines.co.jp/
#nkf -e < marines.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > marines.ntg
#chasen < marines.ntg > marines.txt
#grep 'Ì¾»ì' < marines.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < marines.txt | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O buffaloes.log -o buffaloes.ret http://www.buffaloes.co.jp/
#nkf -e < buffaloes.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > buffaloes.ntg
#chasen < buffaloes.ntg > buffaloes.txt
#grep 'Ì¾»ì' < buffaloes.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < buffaloes.txt | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O eagles.log -o eagles.ret http://www.rakuteneagles.jp/
nkf -e < eagles.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > eagles.ntg
#chasen < eagles.ntg > eagles.txt
#grep 'Ì¾»ì' < eagles.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < eagles.txt | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O dragons.log -o dragons.ret http://dragons.cplaza.ne.jp/
#nkf -e < dragons.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > dragons.ntg
#chasen < dragons.ntg > dragons.txt
#grep 'Ì¾»ì' < dragons.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < dragons.txt | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O tigers.log -o tigers.ret http://www.hanshintigers.jp/
#nkf -e < tigers.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > tigers.ntg
#chasen < tigers.ntg > tigers.txt
#grep 'Ì¾»ì' < tigers.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < tigers.txt | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O swallows.log -o swallows.ret http://www.yakult-swallows.co.jp/
#nkf -e < swallows.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > swallows.ntg
#chasen < swallows.ntg > swallows.txt
#grep 'Ì¾»ì' < swallows.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < swallows.txt | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O giaiants.log -o giants.ret http://www.giants.jp/top.html
#nkf -e < giants.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > giants.ntg
#chasen < giants.ntg> giants.txt
#grep 'Ì¾»ì' < giants.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < giants.txt | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O carp.log -o carp.ret http://www.carp.co.jp/
nkf -e < carp.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > carp.ntg
#chasen < carp.ntg > carp.txt
#grep 'Ì¾»ì' < carp.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < carp.txt | ./chasen2mysql.pl £Î£Ð£Â

#wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O baystars.log -o baystars.ret http://www.baystars.co.jp/
#nkf -e < baystars.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > baystars.ntg
#chasen < baystars.ntg > baystars.txt
#grep 'Ì¾»ì' < baystars.txt | ./chasen2mysql.pl £Î£Ð£Â
#grep 'Ì¤ÃÎ¸ì' < baystars.txt | ./chasen2mysql.pl £Î£Ð£Â

wget -r --no-parent -R jpg,jpeg,gif,png,css,js,cgi,pdf,doc,xls,zip,exe,php,swf -O players.log -o players.ret http://www.sponichi.co.jp/baseball/npb/2007/team/index.html
nkf -e < players.log | removetag | sed 's/ //g' | sed '/^$/d' | jsed 's/¡¡//g' > players.ntg
chasen < players.ntg > players.txt
grep 'Ì¾»ì' < players.txt | ./chasen2mysql.pl £Î£Ð£Â
grep 'Ì¤ÃÎ¸ì' < players.txt | ./chasen2mysql.pl £Î£Ð£Â
