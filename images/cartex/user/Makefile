.if defined(_ER)
DIR =er-blog
.elif defined(_SP)
DIR =sp-blog
.else
DIR =as-blog
.endif
.if defined(_PAY)
PAY = 1
.else
PAY = 0
.endif
SUPDIR=/usr/local/apache/share/${DIR}/images/user

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
instrsv:

instcart:
	install -c -m 644 -o root -g wheel *.gif ${SUPDIR}
	if [ ${PAY} = 1 ]; then (cd ${SUPDIR}; cp logo_p.gif logo.gif; cp title_kekka_p.gif title_kekka.gif) fi
