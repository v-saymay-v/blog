CC = gcc
CFLAGS += -static -g -Wall -O2 \
	-I../../libblog \
	-I../../../excart/include \
	-I../../../lib/include \
	-I/usr/local/include \
	-I/usr/local/include/mysql
.if defined(DEBUG)
CFLAGS += -DDEBUG
.endif
LIBS = -L ../../libblog -lblog \
	-L../../../excart/lib -lexcart \
	-L../../../lib/lib -lnkf -lcgi2 -ljcode -ldbmysql2 -lcommon2 -lnet -lhttp -lrcmndforcart \
	-L/usr/local/lib/mysql -lmysqlclient \
	-L/usr/local/lib -lgd -lpng -ljpeg -lxml2 -liconv -lssl -lcrypto -lcrypt -lmd -lm -lz -lstdc++

TARGET=blog_mobile_admin
SRCS=${TARGET}.c blog_mobile_menu.c blog_mobile_new_entry.c blog_mobile_entry_list.c blog_mobile_modify_entry.c
OBJS=$(SRCS:.c=.o)
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

blog_mobile_admin.o: ../../../lib/include/libnkf.h ../../libblog/libmobile.h
blog_mobile_admin.o: ../../../lib/include/libcgi2.h
blog_mobile_admin.o: ../../../lib/libdb2/libdb2.h
blog_mobile_admin.o: ../../../lib/libcommon2/libcommon2.h
blog_mobile_admin.o: ../../../lib/libcgi2/libcgi2.h ../../libblog/libmysql.h
blog_mobile_admin.o: ../../libblog/libauth.h ../../libblog/blog.h
blog_mobile_admin.o: ../../libblog/libblog.h blog_mobile_menu.h
blog_mobile_admin.o: blog_mobile_new_entry.h blog_mobile_modify_entry.h
blog_mobile_admin.o: blog_mobile_entry_list.h
blog_mobile_menu.o: ../../../lib/include/libcgi2.h
blog_mobile_menu.o: ../../../lib/libdb2/libdb2.h
blog_mobile_menu.o: ../../../lib/libcommon2/libcommon2.h
blog_mobile_menu.o: ../../../lib/libcgi2/libcgi2.h ../../libblog/libblog.h
blog_mobile_menu.o: ../../libblog/blog.h ../../libblog/libmysql.h
blog_mobile_menu.o: ../../libblog/libauth.h ../../libblog/libmobile.h
blog_mobile_menu.o: blog_mobile_menu.h
blog_mobile_new_entry.o: ../../../lib/include/libcgi2.h
blog_mobile_new_entry.o: ../../../lib/libdb2/libdb2.h
blog_mobile_new_entry.o: ../../../lib/libcommon2/libcommon2.h
blog_mobile_new_entry.o: ../../../lib/libcgi2/libcgi2.h
blog_mobile_new_entry.o: ../../libblog/libblog.h ../../libblog/blog.h
blog_mobile_new_entry.o: ../../libblog/libmysql.h ../../libblog/libauth.h
blog_mobile_new_entry.o: ../../libblog/libmobile.h blog_mobile_new_entry.h
blog_mobile_entry_list.o: ../../../lib/include/libcgi2.h
blog_mobile_entry_list.o: ../../../lib/libdb2/libdb2.h
blog_mobile_entry_list.o: ../../../lib/libcommon2/libcommon2.h
blog_mobile_entry_list.o: ../../../lib/libcgi2/libcgi2.h
blog_mobile_entry_list.o: ../../../lib/include/libnkf.h
blog_mobile_entry_list.o: ../../libblog/libmysql.h ../../libblog/libauth.h
blog_mobile_entry_list.o: ../../libblog/blog.h ../../libblog/libmobile.h
blog_mobile_entry_list.o: ../../libblog/libblog.h blog_mobile_entry_list.h
blog_mobile_modify_entry.o: ../../../lib/include/libcgi2.h
blog_mobile_modify_entry.o: ../../../lib/libdb2/libdb2.h
blog_mobile_modify_entry.o: ../../../lib/libcommon2/libcommon2.h
blog_mobile_modify_entry.o: ../../../lib/libcgi2/libcgi2.h
blog_mobile_modify_entry.o: ../../libblog/libblog.h ../../libblog/blog.h
blog_mobile_modify_entry.o: ../../libblog/libmysql.h ../../libblog/libauth.h
blog_mobile_modify_entry.o: ../../libblog/libmobile.h blog_mobile_new_entry.h
