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
instrsv:

instcart:
	install -c -m 644 -o root -g wheel *.css ${TAGDIR}
