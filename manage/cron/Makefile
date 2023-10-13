CC = gcc
CFLAGS += -static -g -Wall -O2 \
	-I../../libblog \
	-I../../../excart/include -I../../../lib/include \
	-I/usr/local/include \
	-I/usr/local/include/mysql
.if defined(DEBUG)
CFLAGS += -DDEBUG
.endif
LIBS = -L ../../libblog -lblog \
	-L../../../lib/lib -L../../../excart/lib -lexcart -lnkf -lcgi2 -ljcode -ldbmysql2 -lcommon2 -lnet -lhttp -lgd -lpng -ljpeg -lrcmndforcart \
	-L/usr/local/lib/mysql -lmysqlclient \
	-L/usr/local/lib -lxml2 -liconv -lssl -lcrypto -lcrypt -lmd -lm -lz -lstdc++ -lstdc++

SRCS=blog_cron.c
OBJS=blog_cron.o
TARGET=blog_cron
TAGFILE=/usr/local/sbin/${TARGET}
BLOGLIB=../../libblog/libblog.a
TODAY=`date +%Y%m%d%H%M`
MKDIR = /bin/mkdir -p

${TARGET}: ${OBJS} ${BLOGLIB}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

all: clean ${TARGET}

clean:
	rm -f ${TARGET} *.o

instdbb:
	$(MKDIR) /home/blog/backups
	$(MKDIR) /home/blog/backups/$(TODAY)
	install -c -o blog -m 700 ${TAGFILE} /home/blog/backups/$(TODAY)
	install -c -m 700 -o root -g wheel ${TARGET} ${TAGFILE}

instcart:
	install -c -m 700 -o root -g wheel ${TARGET} ${TAGFILE}

instrsv:
	install -c -m 700 -o root -g wheel ${TARGET} ${TAGFILE}

insthb:
	install -c -m 700 -o root -g wheel ${TARGET} ${TAGFILE}

hbsync:
cartsync:
rsvsync:
dbbsync:

depend:
	makedepend -Y /usr/include -Y /usr/local/include  -Y /usr/local/include/mysql -I ../../libblog -I ../../../lib/include -- $(SRCS)
# DO NOT DELETE

blog_cron.o: ../../../lib/include/libcommon2.h ../../../lib/libdb2/libdb2.h
blog_cron.o: ../../../lib/libcgi2/libcgi2.h
blog_cron.o: ../../../lib/libcommon2/libcommon2.h ../../libblog/libblog.h
blog_cron.o: ../../../lib/include/libcgi2.h ../../libblog/blog.h
blog_cron.o: ../../libblog/libmysql.h
