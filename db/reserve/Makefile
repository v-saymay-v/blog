.if defined(_ER)
DIR =er-blog
.elif defined(_SP)
DIR =sp-blog
.else
DIR =as-blog
.endif

TAGFILE=/usr/local/share/skel/as-blog/${DIR}.sql
FILE1=create_table.sql

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
	install -c -m 644 ${FILE1} ${TAGFILE}
