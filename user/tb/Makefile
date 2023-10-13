CC = gcc
CFLAGS += -static -g -Wall -O2 \
	-I../../libblog \
	-I../../../excart/include -I../../../lib/include \
	-I/usr/local/include \
	-I/usr/local/include/mysql
LIBS = -L ../../libblog -lblog \
	-L../../../lib/lib -L../../../excart/lib -lexcart -lnkf -lcgi2 -ljcode -ldbmysql2 -lcommon2 -lnet -lhttp -lgd -lpng -ljpeg -lrcmndforcart \
	-L/usr/local/lib/mysql -lmysqlclient \
	-L/usr/local/lib -lxml2 -liconv -lssl -lcrypto -lcrypt -lcrypt -lmd -lm -lz -lstdc++

.if defined(DEBUG)
CFLAGS += -DDEBUG
.endif

SRCS=tb.c
OBJS=tb.o
TARGET=tb
LOCALHB=${HOME}/public_html/ver46/blog/${TARGET}.cgi
LOCALCART=${HOME}/public_html/e-commex/${TARGET}.cgi
LOCALRSV=${HOME}/public_html/reserve/${TARGET}.cgi
LOCALDBB=${HOME}/public_html/dbb_blog/${TARGET}.cgi
TARGETDBB=/home/blog/public_html/cgi-bin/user/${TARGET}.cgi
TARGETCART=/usr/local/apache/share/e-commex/${TARGET}.cgi
TARGETRSV=/usr/local/apache/share/reserve/${TARGET}.cgi
TARGETHB=/usr/local/apache/hotbiz/share/blog/${TARGET}.cgi
BLOGLIB=../../libblog/libblog.a
TODAY=`date +%Y%m%d%H%M`
MKDIR = /bin/mkdir -p

${TARGET}.cgi: ${OBJS} ${BLOGLIB}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

all: clean ${TARGET}.cgi

clean:
	rm -f ${TARGET}.cgi *.o

hbsync:
	cp ${TARGET}.cgi ${LOCALHB}

cartsync:
	cp ${TARGET}.cgi ${LOCALCART}

rsvsync:
	cp ${TARGET}.cgi ${LOCALRSV}

dbbsync:
	cp ${TARGET}.cgi ${LOCALDBB}

instdbb:
	$(MKDIR) /home/blog/backups
	$(MKDIR) /home/blog/backups/$(TODAY)
	install -c -o blog -m 0755 ${TARGETDBB} /home/blog/backups/$(TODAY)
	install -c -m 755 -o blog ${TARGET}.cgi ${TARGETDBB}

instcart:
	install -c -m 755 -o root -g wheel ${TARGET}.cgi ${TARGETCART}

instrsv:
	install -c -m 755 -o root -g wheel ${TARGET}.cgi ${TARGETRSV}

insthb:
	install -c -m 755 -o root -g wheel ${TARGET}.cgi ${TARGETHB}

depend:
	makedepend -Y /usr/include -Y /usr/local/include  -Y /usr/local/include/mysql -I ../../libblog -I ../../../lib/include -- $(SRCS)
# DO NOT DELETE

tb.o: ../../../lib/include/libcgi2.h ../../../lib/libdb2/libdb2.h
tb.o: ../../../lib/libcommon2/libcommon2.h ../../../lib/libcgi2/libcgi2.h
tb.o: ../../../excart/include/excart.h ../../../excart/include/libexcart.h
tb.o: ../../../lib/include/libnkf.h ../../../lib/include/libnet.h
tb.o: ../../../lib/include/libcommon2.h ../../../lib/include/libhttp.h
tb.o: ../../libblog/libauth.h ../../libblog/blog.h ../../libblog/libmysql.h
tb.o: ../../libblog/libblog.h
