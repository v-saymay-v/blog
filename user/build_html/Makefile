CC = gcc
CFLAGS += -static -g -Wall -O2 \
	-I../../libblog \
	-I../../../lib/include \
	-I../../../excart/include \
	-I/usr/local/include \
	-I/usr/local/include/mysql
.if defined(DEBUG)
CFLAGS += -DDEBUG
.endif
LIBS = -L ../../libblog -lblog \
	-L../../../excart/lib -lexcart \
	-L../../../lib/lib -lnkf -lcgi2 -ljcode -ldbmysql2 -lcommon2 -lnet -lhttp -lgd -lpng -ljpeg -lrcmndforcart \
	-L/usr/local/lib/mysql -lmysqlclient \
	-L/usr/local/lib -lxml2 -liconv -lgd -lpng -ljpeg -lssl -lcrypto -lcrypt -lmd -lm -lz -lstdc++

SRCS=blog_build_html.c
OBJS=blog_build_html.o
TARGET=blog_build_html
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
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

instcart:
	install -c -m 755 -o root -g wheel ${TARGET}.cgi ${TARGETCART}
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

instrsv:
	install -c -m 755 -o root -g wheel ${TARGET}.cgi ${TARGETRSV}
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

insthb:
	install -c -m 755 -o root -g wheel ${TARGET}.cgi ${TARGETHB}
	@for dir in *; do \
		if [ ! -d "$$dir" ]; then continue; fi; \
		for ignore in $(IGNORE_DIRS); do \
			if [ "$$dir" = "$$ignore" ]; then continue 2; fi \
		done; \
		if [ -f "$$dir/Makefile" ]; then (cd $$dir; pwd; $(MAKE) $@); fi \
	done;

depend:
	makedepend -Y /usr/include -Y /usr/local/include  -Y /usr/local/include/mysql -I ../../libblog -I ../../../lib/include -- $(SRCS)
# DO NOT DELETE

blog_build_html.o: ../../../lib/include/libcgi2.h
blog_build_html.o: ../../../lib/libdb2/libdb2.h
blog_build_html.o: ../../../lib/libcommon2/libcommon2.h
blog_build_html.o: ../../../lib/libcgi2/libcgi2.h
blog_build_html.o: ../../../excart/include/excart.h
blog_build_html.o: ../../../excart/include/libexcart.h
blog_build_html.o: ../../../lib/include/libnkf.h
blog_build_html.o: ../../../lib/include/libjcode.h
blog_build_html.o: ../../../lib/include/libdb2.h
blog_build_html.o: ../../../lib/include/libcommon2.h
blog_build_html.o: ../../../lib/include/libnet.h
blog_build_html.o: ../../../lib/include/libhttp.h ../../libblog/libauth.h
blog_build_html.o: ../../libblog/blog.h ../../libblog/libmysql.h
blog_build_html.o: ../../libblog/libblog.h ../../libblog/libblogcart.h
blog_build_html.o: ../../libblog/libblogreserve.h
