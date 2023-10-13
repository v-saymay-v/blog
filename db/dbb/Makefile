SRC1=edit_dic.cgi
SRC3=connect.cha
TARGET1=/home/blog/public_html/cgi-bin/asp/${SRC1}
TARGET3=/usr/local/share/chasen/dic/ipadic/${SRC3}

default:
all:
clean:
hbsync:
cartsync:
rsvsync:
dbbsync:
depend:
insthb:
instcart:
instrsv:

instdbb: ${TARGET3}
	install -c -m 755 -o blog ${SRC1} ${TARGET1}

${TARGET3}: connect_dbb.txt connect_mlb.txt connect_ng.txt connect_npb.txt connect_yougo.txt
	cp -p /usr/local/share/chasen/dic/ipadic/${SRC3}.orig /usr/local/share/chasen/dic/ipadic/${SRC3}
	cat connect_*.txt >> ${TARGET3}
	(cd /usr/local/share/chasen/dic/ipadic; /usr/local/libexec/chasen/makemat; /usr/local/libexec/chasen/makeda dbbdic dbbdic.dict)
