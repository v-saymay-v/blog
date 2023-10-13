.if defined(_ER)
DIR =er-blog
.elif defined(_SP)
DIR =sp-blog
.else
DIR =as-blog
.endif
SUPDIR=/usr/local/apache/share/${DIR}/skeleton

default:
all:
clean:
hbsync:
cartsync:
rsvsync:
dbbsync:
depend:
instdbb:
insthb:
instcart:

instrsv:
	install -c -m 644 -o root -g wheel *.skl ${SUPDIR}
