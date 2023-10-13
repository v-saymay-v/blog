CC ?= gcc
AR = ar
RM = rm -f
RL = ranlib
CFLAGS += -Wall -I. \
	-I../../excart/include \
	-I../../lib/include \
	-I/usr/local/pgsql/include \
	-I/usr/local/include/mysql \
	-I/usr/local/include/libxml2 \
	-I/usr/local/include

.if defined(DEBUG)
.if defined(PROFILE)
CFLAGS += -pg -DDEBUG
.else
CFLAGS += -g -DDEBUG
.endif
.endif


SRCS = libmysql.c libblog.c libauth.c libblogcart.c dispcart.c libblogreserve.c libmobile.c
OBJS = $(SRCS:.c=.o)
HEDS = $(SRCS:.c=.h) blog.h
PROGRAM  = libblog.a
ALLTARGET= $(PROGRAM)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(PROGRAM) :$(OBJS)
	$(AR) r $(PROGRAM) $(OBJS)
	$(RL) $(PROGRAM)

all:clean $(PROGRAM)

clean:
	rm -f *.a a.out *.o *.x *.BAK *.bak *.core *~ .*~ \#* $(ALLTARGET) \
		.depend
	$(RM) $(OBJS)
	$(RM) $(PROGRAM1) $(PROGRAM2)

depend:
	makedepend -Y /usr/include -Y /usr/local/include  -Y /usr/local/include/mysql -Y /usr/local/include/fcgi -I../../excart/include -I ../../lib/include -- $(SRCS)

instdbb:
instcart:
instrsv:
insthb:
hbsync:
cartsync:
rsvsync:
dbbsync:
# DO NOT DELETE

libmysql.o: libmysql.h
libblog.o: libauth.h blog.h libmysql.h libblog.h
libauth.o: libblog.h blog.h libmysql.h libauth.h
libblogcart.o: libblog.h blog.h libmysql.h libauth.h dispcart.h libblogcart.h
dispcart.o: libmysql.h libblog.h blog.h dispcart.h
libblogreserve.o: libblog.h blog.h libmysql.h libauth.h libblogreserve.h
libmobile.o: libblog.h blog.h libmysql.h libauth.h libmobile.h
