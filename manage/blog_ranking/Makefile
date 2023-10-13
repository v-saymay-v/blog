CC = gcc
CFLAGS += -static -g -Wall -O2 \
	-I../../libblog \
	-I../../../lib/include \
	-I/usr/local/include/mysql \
	-I/usr/local/include
.if defined(DEBUG)
CFLAGS += -DDEBUG
.endif
LIBS = -L ../../libblog -lblog \
	-L../../../excart/lib -lexcart \
	-L../../../lib/lib -lexcart -lnkf -lcgi2 -ljcode -ldbmysql2 -lcommon2 -lnet -lhttp -lrcmndforcart \
	-L/usr/local/lib/mysql -lmysqlclient \
	-L/usr/local/lib -lgd -ljpeg -lpng -lxml2 -liconv -lssl -lcrypto -lcrypt -lmd -lm -lz -lstdc++

TARGET=blog_ranking
SRCS=${TARGET}.c
OBJS=${TARGET}.o
BLOGLIB=../../libblog/libblog.a

${TARGET}.cgi: ${OBJS} ${BLOGLIB}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

all: clean ${TARGET}.cgi

clean:
	rm -f ${TARGET}.cgi *.o

hbsync:
cartsync:
rsvsync:
dbbsync:
instdbb:
instcart:
instrsv:
insthb:

depend:
	makedepend -Y /usr/include -Y /usr/local/include  -Y /usr/local/include/mysql -I ../../libblog -I ../../../lib/include -- $(SRCS)
# DO NOT DELETE

blog_ranking.o: ../../../lib/include/libcgi2.h ../../../lib/libdb2/libdb2.h
blog_ranking.o: ../../../lib/libcommon2/libcommon2.h
blog_ranking.o: ../../../lib/libcgi2/libcgi2.h ../../libblog/libmysql.h
blog_ranking.o: ../../../lib/include/libcommon2.h ../../libblog/libauth.h
blog_ranking.o: ../../libblog/blog.h ../../libblog/libblog.h
blog_ranking.o: ../../libblog/libblogcart.h ../../libblog/libblogreserve.h
