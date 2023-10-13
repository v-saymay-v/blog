CC = gcc
CFLAGS += -static -g -Wall -O2 \
	-I../../libblog \
	-I../../../excart/include -I../../../lib/include \
	-I/usr/local/include/mysql \
	-I/usr/local/include
.if defined(DEBUG)
CFLAGS += -DDEBUG
.endif
LIBS = -L ../../libblog -lblog \
	-L../../../lib/lib -L../../../excart/lib -lexcart -lnkf -lcgi2 -ljcode -ldbmysql2 -lcommon2 -lnet -lhttp -lgd -lpng -ljpeg -lrcmndforcart \
	-L/usr/local/lib/mysql -lmysqlclient \
	-L/usr/local/lib -lxml2 -liconv -lssl -lcrypto -lcrypt -lmd -lm -lz -lstdc++

SRCS=blog_menu.c
OBJS=blog_menu.o
TARGET=blog_menu
LOCALHB=${HOME}/public_html/ver46/blog/${TARGET}.cgi
LOCALCART=${HOME}/public_html/e-commex/admin/${TARGET}.cgi
LOCALRSV=${HOME}/public_html/reserve/admin/${TARGET}.cgi
LOCALDBB=${HOME}/public_html/dbb_blog/${TARGET}.cgi
TARGETDBB=/home/blog/public_html/cgi-bin/user/${TARGET}.cgi
TARGETCART=/usr/local/apache/share/e-commex/admin/${TARGET}.cgi
TARGETRSV=/usr/local/apache/share/reserve/admin/${TARGET}.cgi
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

blog_menu.o: ../../../lib/include/libcgi2.h ../../../lib/libdb2/libdb2.h
blog_menu.o: ../../../lib/libcommon2/libcommon2.h
blog_menu.o: ../../../lib/libcgi2/libcgi2.h ../../../lib/include/libdb2.h
blog_menu.o: ../../../lib/include/libcommon2.h ../../libblog/libauth.h
blog_menu.o: ../../libblog/blog.h ../../libblog/libmysql.h
blog_menu.o: ../../libblog/libblog.h ../../libblog/libblogcart.h
blog_menu.o: ../../libblog/libblogreserve.h
