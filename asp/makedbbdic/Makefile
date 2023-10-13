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

SRCS=makedbbdic.c
OBJS=makedbbdic.o
TARGET=makedbbdic
TAGFILE=/usr/local/libexec/chasen/${TARGET}

${TARGET}: ${OBJS} ${BLOGLIB}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

all: clean ${TARGET}

clean:
	rm -f ${TARGET} *.o

instdbb:
	install -c -m 4555 -o root ${TARGET} ${TAGFILE}

hbsync:
cartsync:
dbbsync:
rsvsync:
instcart:
insthb:
instrsv:

depend:
	makedepend -Y /usr/include -Y /usr/local/include  -Y /usr/local/include/mysql -I ../../libblog -I ../../../lib/include -- $(SRCS)
# DO NOT DELETE
