CC=cc
CFLAGS=-g -static -Wall -O2 -I../../libblog -I../../../excart/include -I../../../lib/include -I/usr/local/include -I/usr/local/include/mysql
LIBS=-L../../libblog -lblog -L../../../lib/lib -L../../../excart/lib -lexcart -lnkf -ldbmysql2 -lcgi2 -ljcode -lcommon2 -lnet -lhttp -L/usr/local/lib/mysql -lmysqlclient -L/usr/local/lib -lgd -ljpeg -lpng -lrcmndforcart -lssl -lcrypto -lxml2 -liconv -lcrypt -lmd -lm -lz -lstdc++

SRCS=blog_createblog.c
OBJS=blog_createblog.o
TARGET=blog_createblog
BLOGLIB=../../libblog/libblog.a

${TARGET}.cgi: ${OBJS} ${BLOGLIB}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

all: clean ${TARGET}.cgi

clean:
	rm -f ${TARGET}.cgi *.o

hbsync:
cartsync:
dbbsync:
rsvsync:
instdbb:
instcart:
insthb:
instrsv:

depend:
	makedepend -Y /usr/include -Y /usr/local/include  -Y /usr/local/include/mysql -I ../../libblog -I ../../../lib/include -- $(SRCS)
# DO NOT DELETE

blog_createblog.o: ../../libblog/libblog.h ../../../lib/include/libcgi2.h
blog_createblog.o: ../../../lib/libdb2/libdb2.h
blog_createblog.o: ../../../lib/libcommon2/libcommon2.h
blog_createblog.o: ../../../lib/libcgi2/libcgi2.h ../../libblog/blog.h
blog_createblog.o: ../../libblog/libmysql.h ../../../lib/include/libcommon2.h
