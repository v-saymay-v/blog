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

SRCS=asjseo.c
OBJS=asjseo.o
TARGET=asjseo.cgi
LOCALTARGET=${HOME}/public_html/${TARGET}

${TARGET}: ${OBJS} ${BLOGLIB}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

all: clean ${TARGET}

clean:
	rm -f ${TARGET} *.o

cartsync:
	cp ${TARGET} ${LOCALTARGET}

hbsync:
dbbsync:
rsvsync:
instcart:
instdbb:
insthb:
instrsv:

depend:
	makedepend -Y /usr/include -Y /usr/local/include  -Y /usr/local/include/mysql -I ../../libblog -I ../../../lib/include -- $(SRCS)
# DO NOT DELETE

asjseo.o: ../../../lib/include/libcommon2.h ../../../lib/libdb2/libdb2.h
asjseo.o: ../../../lib/libcgi2/libcgi2.h ../../../lib/libcommon2/libcommon2.h
asjseo.o: ../../../lib/include/libnet.h ../../../lib/include/libnkf.h
asjseo.o: ../../libblog/libmysql.h
