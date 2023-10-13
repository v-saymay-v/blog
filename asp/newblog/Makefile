CC=cc
CFLAGS=-g -static -Wall -O2 -I../../libblog -I../../../excart/include -I../../../lib/include -I/usr/local/include -I/usr/local/include/mysql
LIBS=-L ../../libblog -lblog -L../../../lib/lib -L../../../excart/lib -lexcart -lnkf -lcgi2 -ljcode -ldbmysql2 -lcommon2 -lnet -lhttp -L/usr/local/lib/mysql -lmysqlclient -L/usr/local/lib -lgd -ljpeg -lpng -lrcmndforcart -lssl -lcrypto -lxml2 -liconv -lcrypt -lmd -lm -lz -lstdc++

.if defined(DEBUG)
CFLAGS += -DDEBUG
.endif
.if defined(DBBTEST)
CFLAGS += -DDBBTEST
.endif
.if defined(HORITEST)
CFLAGS += -DHORITEST
.endif

SRCS=newblog.c
OBJS=newblog.o
TARGET=newblog
TARGETFILE=/usr/local/sbin/${TARGET}
BLOGLIB=../../libblog/libblog.a

${TARGET}: ${OBJS} ${BLOGLIB}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

all: clean ${TARGET}

clean:
	rm -f ${TARGET} *.o

instcart:
	install -c -m 500 -o root -g wheel ${TARGET} ${TARGETFILE}
instrsv:
	install -c -m 555 -o root -g wheel ${TARGET} ${TARGETFILE}

hbsync:
cartsync:
dbbsync:
rsvsync:
instdbb:
insthb:

depend:
	makedepend -Y /usr/include -Y /usr/local/include  -Y /usr/local/include/mysql -I ../../libblog -I ../../../lib/include -- $(SRCS)
# DO NOT DELETE

newblog.o: ../../../lib/include/libcommon2.h ../../../lib/libdb2/libdb2.h
newblog.o: ../../../lib/libcgi2/libcgi2.h
newblog.o: ../../../lib/libcommon2/libcommon2.h ../../libblog/libblog.h
newblog.o: ../../../lib/include/libcgi2.h ../../libblog/blog.h
newblog.o: ../../libblog/libmysql.h
