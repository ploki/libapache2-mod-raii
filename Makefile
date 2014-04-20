
#CXX=clang++
#CC=clang
#CXX=g++ -std=gnu++0x
#CC=gcc
CXX=c++
CC=cc


APXS=apxs2
APACHECTL=apache2ctl

APR=apr-config

# Get all of apxs's internal values.
APXS_CC=`$(APXS) -q CC`
APXS_CFLAGS=`$(APXS) -q CFLAGS`
APXS_CFLAGS_SHLIB=`$(APXS) -q CFLAGS_SHLIB`
APXS_INCLUDEDIR=`$(APXS) -q INCLUDEDIR`
APXS_LD_SHLIB=`$(APXS) -q LD_SHLIB`
APXS_LDFLAGS_SHLIB=`$(APXS) -q LDFLAGS_SHLIB`
APXS_LIBEXECDIR=`$(APXS) -q LIBEXECDIR`
APXS_LIBS_SHLIB=`$(APXS) -q LIBS_SHLIB`
APXS_SBINDIR=`$(APXS) -q SBINDIR`
APXS_SYSCONFDIR=`$(APXS) -q SYSCONFDIR`
APXS_TARGET=`$(APXS) -q TARGET`

APR_CC=`$(APR) --cc`
APR_CPP=`$(APR) --cpp`
APR_CFLAGS=`$(APR) --cflags`
APR_CPPFLAGS=`$(APR) --cppflags`
APR_INCLUDES=`$(APR) --includes`
APR_LDFLAGS=`$(APR) --ldflags`
APR_LIBS=`$(APR) --libs`
APR_LINK_LD=`$(APR) --link-ld`



MOD_CFILES:=mod_raii.C

RAII_CFILES:=config.C raii.C \
	Object.C \
	ApacheInteraction.C \
	ApacheSubRequest.C \
	AttributeHolder.C \
	Cookie.C \
	GetRequestDispatcher.C \
	ServletContext.C \
	ServletConfig.C \
	ServletContainer.C \
	HttpServlet.C \
	HttpServletRequest.C \
	HttpServletResponse.C \
	HttpSession.C \
	RequestDispatcher.C \
	DefaultTags.C \
	Connection.C \
	Begin.C \
	SQLDriverContainer.C \
	ResultSet.C \
	ResultSetImpl.C \
	Exception.C \
	segfault.C \
	String.C \
	Regex.C


HAVE_TLS=`getconf GNU_LIBPTHREAD_VERSION | grep NPTL >/dev/null && echo -DHAVE_TLS`

MOD_OFILES:=$(MOD_CFILES:.C=.o)
RAII_OFILES:=$(RAII_CFILES:.C=.o)

.PHONY: all .raiipp

all: libraii.so mod_raii.so .raiipp \
     libraii_sqldriver_pgsql.so libraii_sqldriver_sqlite.so \
     libraii_cairo.so \
     raiilog
#libraii_sqldriver_sqlrelay.so

.raiipp:
	+make -C raiipp
raiilog: raiilog.c
	$(CC) -Wall -Werror -D_GNU_SOURCE -o $@ $<

mod_raii.so: $(MOD_OFILES)
	$(CXX) -ggdb3 -Wl,-E -fPIC -shared -o $@ $(MOD_OFILES) $(APXS_LIBS_SHLIB) $(APR_LINK_LD) -L. -lraii

libraii.so: $(RAII_OFILES)
	$(CXX) -ggdb3 -Wl,-E -fPIC -shared -o $@ $(RAII_OFILES) $(APXS_LIBS_SHLIB) $(APR_LINK_LD)


libraii_sqldriver_pgsql.so: SQLDriverPGSQL.o
	$(CXX) -ggdb3 -Wl,-E -fPIC -shared -o $@ $< -lpq

libraii_sqldriver_sqlrelay.so: SQLDriverSQLR.o
	$(CXX) -ggdb3 -Wl,-E -fPIC -shared -o $@ $< -lsqlrclient

libraii_sqldriver_sqlite.so: SQLDriverSQLITE.o
	$(CXX) -ggdb3 -Wl,-E -fPIC -shared -o $@ $< -lsqlite

Cairo.o: Cairo.C Cairo.H
	$(CXX) $(CPPFLAGS) -Wl,-E -fPIC -shared -c -o $@ Cairo.C `pkg-config pangocairo --cflags`

libraii_cairo.so: Cairo.o BoxedText.o
	$(CXX) $(CPPFLAGS) -Wl,-E -fPIC -shared -o $@ Cairo.o BoxedText.o `pkg-config pangocairo --libs`


#NO_INLINE=-fno-inline -DNO_INLINE
NO_INLINE=-fno-inline
#NO_INLINE=

#DEBUG_FLAGS=-DDEBUG_ALLOC
DEBUG_FLAGS= 


CPPFLAGS=$(NO_INLINE) $(DEBUG_FLAGS) -ggdb3 -fPIC $(HAVE_TLS) -D_REENTRANT -I. \
	-I $(APXS_INCLUDEDIR) \
	-I $(APR_INCLUDES) \
	-I/usr/include/postgresql \
	$(APXS_CFLAGS) $(APXS_CFLAGS_SHLIB) $(APR_CPPFLAGS) $(APR_CFLAGS) $(APR_INCLUDES) -Wall -Werror\
	-D__IN_MODULE__

install: all
	$(SU0) install -d $(DESTDIR)$(APXS_LIBEXECDIR)
	$(SU0) install mod_raii.so $(DESTDIR)$(APXS_LIBEXECDIR)
	$(SU0) install -d $(DESTDIR)/usr/lib
	$(SU0) install libraii.so $(DESTDIR)/usr/lib
	$(SU0) install libraii_sqldriver_*.so $(DESTDIR)/usr/lib
	$(SU0) install libraii_cairo.so $(DESTDIR)/usr/lib
	$(SU0) install -d $(DESTDIR)/usr/include/raii
	$(SU0) install -m 644 *.H $(DESTDIR)/usr/include/raii
	$(SU0) install -d $(DESTDIR)/usr/bin
	$(SU0) install RaiiBuild.sh $(DESTDIR)/usr/bin/RaiiBuild
	$(SU0) install raiipp/raiipp $(DESTDIR)/usr/bin
	$(SU0) install -d $(DESTDIR)/usr/sbin
	$(SU0) install raiilog $(DESTDIR)/usr/sbin

# display the apxs variables
check_apxs_vars:
	@echo APXS_CC $(APXS_CC);\
	echo APXS_CFLAGS $(APXS_CFLAGS);\
	echo APXS_CFLAGS_SHLIB $(APXS_CFLAGS_SHLIB);\
	echo APXS_INCLUDEDIR $(APXS_INCLUDEDIR);\
	echo APXS_LD_SHLIB $(APXS_LD_SHLIB);\
	echo APXS_LDFLAGS_SHLIB $(APXS_LDFLAGS_SHLIB);\
	echo APXS_LIBEXECDIR $(APXS_LIBEXECDIR);\
	echo APXS_LIBS_SHLIB $(APXS_LIBS_SHLIB);\
	echo APXS_SBINDIR $(APXS_SBINDIR);\
	echo APXS_SYSCONFDIR $(APXS_SYSCONFDIR);\
	echo APXS_TARGET $(APXS_TARGET);

# display the apr variables
check_apr_vars:
	@echo APR_CC $(APR_CC);\
	echo APR_CPP $(APR_CPP);\
	echo APR_CFLAGS $(APR_CFLAGS);\
	echo APR_CPPFLAGS $(APR_CPPFLAGS);\
	echo APR_INCLUDES $(APR_INCLUDES);\
	echo APR_LDFLAGS $(APR_LDFLAGS); \
	echo APR_LIBS $(APR_LIBS); \
	echo APR_LINK_LD $(APR_LINK_LD); \


check_vars: check_apxs_vars check_apr_vars


#   cleanup
clean:
	make -C raiipp clean
	-rm -f *.so *.o *~ raiilog

