.if defined(_ER)
DIR =er-blog
.elif defined(_SP)
DIR =sp-blog
.else
DIR =as-blog
.endif
TARGETDBB=/home/blog/public_html/scripts
TARGETCART=/usr/local/apache/share/${DIR}/scripts
TARGETRSV=/usr/local/apache/share/${DIR}/scripts
TARGETHB=/usr/local/apache/hotbiz/jscripts

default:
all:
clean:
depend:
hbsync:
cartsync:
rsvsync:
dbbsync:
instdbb:
	install -c -m 644 -o blog *.gif ${TARGETDBB}
	install -c -m 644 -o blog *.js ${TARGETDBB}
	install -c -m 644 -o blog *.html ${TARGETDBB}
instcart:
	install -c -m 644 -o root -g wheel *.gif ${TARGETCART}
	install -c -m 644 -o root -g wheel *.js ${TARGETCART}
	install -c -m 644 -o root -g wheel *.html ${TARGETCART}
instrsv:
	install -c -m 644 -o root -g wheel *.gif ${TARGETCART}
	install -c -m 644 -o root -g wheel *.js ${TARGETCART}
	install -c -m 644 -o root -g wheel *.html ${TARGETCART}
insthb:
	install -c -m 644 -o root -g wheel *.gif ${TARGETHB}
	install -c -m 644 -o root -g wheel *.js ${TARGETHB}
	install -c -m 644 -o root -g wheel *.html ${TARGETHB}
