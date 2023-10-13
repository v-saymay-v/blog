CC=cc
CFLAGS=-g -Wall -O2 -I../../libblog -I../../../excart/include -I../../../lib/include -I/usr/local/include -I/usr/local/include/mysql
LIBS=-L../../libblog -L../../../lib/lib -L../../../excart/lib -lblog -lexcart -lnkf -lcgi2 -ljcode -ldbmysql2 -lcommon2 -lnet -lhttp -L/usr/local/lib/mysql -lmysqlclient -L/usr/local/lib -lgd -ljpeg -lpng -lrcmndforcart -lssl -lcrypto -lxml2 -liconv -lcrypt -lmd -lm -lz -lstdc++

.if defined(DEBUG)
CFLAGS += -DDEBUG
.endif
.if defined(DBBTEST)
CFLAGS += -DDBBTEST
.endif
.if defined(HORITEST)
CFLAGS += -DHORITEST
.endif

SRCS=seoreport.c
OBJS=seoreport.o
TARGET=seoreport
TARGETFILE=/usr/local/sbin/${TARGET}

${TARGET}: ${OBJS} ${BLOGLIB}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

all: clean ${TARGET}

clean:
	rm -f ${TARGET} *.o

instcart:
	install -c -m 700 -o root -g wheel ${TARGET} ${TARGETFILE}

cartsync:
hbsync:
dbbsync:
rsvsync:
instdbb:
insthb:
instrsv:

depend:
	makedepend -Y /usr/include -Y /usr/local/include  -Y /usr/local/include/mysql -I ../../libblog -I ../../../lib/include -- $(SRCS)
# DO NOT DELETE

seoreport.o: ../../../lib/include/libcommon2.h ../../../lib/libdb2/libdb2.h
seoreport.o: ../../../lib/libcgi2/libcgi2.h
seoreport.o: ../../../lib/libcommon2/libcommon2.h
seoreport.o: ../../../lib/include/libnet.h ../../../lib/include/libnkf.h
seoreport.o: ../../libblog/libmysql.h
