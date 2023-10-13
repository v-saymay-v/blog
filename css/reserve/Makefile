.if defined(_ER)
DIR =er-blog
.elif defined(_SP)
DIR =sp-blog
.else
DIR =as-blog
.endif
TAGDIR=/usr/local/apache/share/${DIR}/css

default:
all:
clean:
hbsync:
cartsync:
rsvsync:
dbbsync:
depend:
insthb:
instdbb:
instcart:

instrsv:
	install -c -m 644 -o root -g wheel *.css ${TAGDIR}
	install -c -m 755 -o root -g wheel as2er.sh ${TAGDIR}
