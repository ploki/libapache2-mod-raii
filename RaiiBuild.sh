#! /bin/bash

#CPLUSPLUS=clang++
#CPLUSPLUSFLAGS="-std=gnu++0x"
CPLUSPLUS=c++

if getconf GNU_LIBPTHREAD_VERSION | grep NPTL >/dev/null ; then
  HAVE_TLS="-DHAVE_TLS"
else
  HAVE_TLS=""
fi

if [ -z "$HAVE_TLS" ] ; then
 TLS_SUFFIX="tsd"
else
 TLS_SUFFIX="tls"
fi


#NO_INLINE="-fno-inline -DNO_INLINE"
NO_INLINE="-fno-inline"
#NO_INLINE=

#RAIIDIR=/home/guigui/sa/raii/libapache2-mod-raii
RAIIDIR=/usr/include/raii

CPLUSPLUSFLAGS="$CPLUSPLUSFLAGS -march=native -mfpmath=sse $HAVE_TLS $NO_INLINE -D_REENTRANT -D_GNU_SOURCE -ggdb3  -fPIC -DPIC  -I$RAIIDIR -I `apxs2 -q INCLUDEDIR` `apr-config --includes` `apxs2 -q CFLAGS` `apr-config --cppflags` `apr-config --cflags` -Wall"

LINKFLAGS="-Wl,-E -fPIC `apxs2 -q CFLAGS_SHLIB` -shared"

if [ $# -eq 0 ] ; then
  echo "$CPLUSPLUS"
  exit 0
fi

if [ "$1" = "--cplusplusflags" ] ; then
  echo "-I.. $CPLUSPLUSFLAGS"
  exit 0
fi

if [ "$1" = "--linkflags" ] ; then
  echo "$LINKFLAGS -lraii"
  exit 0
fi

ARCH="$1"
TMPDIR="$2"
DSODIR="$3"
RAIIFILE="$4"
CFILE="${TMPDIR}/${RAIIFILE}.C"
DATE=$(stat -c %Y "$RAIIFILE")
SOFILE="${DSODIR}/${RAIIFILE}.${TLS_SUFFIX}.${ARCH}.so.${DATE}"

BUILDOPT="$(dirname $RAIIFILE)/.build"
if [ -r "$BUILDOPT" ] ; then
  CPLUSPLUSFLAGS="$(cat "$BUILDOPT") $CPLUSPLUSFLAGS"
fi

mkdir -p "$(dirname "$CFILE")" "$(dirname "$SOFILE" )"

if [ "$SOFILE" -nt "$RAIIFILE" ] ; then
	echo "$0: « $SOFILE » is up to date." >&2
	exit 0
fi

rm -f "$SOFILE" "$CFILE"


if [ "${RAIIFILE//*./}" = "csp" ] ; then
    raiipp "$RAIIFILE" "$CFILE"
    RET=$?
    if [ $RET -ne 0 ] ; then
	exit $RET
    fi
else
    CFILE="$RAIIFILE"
fi
TICKET=$(dd if=/dev/urandom bs=1 count=8 2>/dev/null | hexdump -v -e '/1 "%02X"')

$CPLUSPLUS "-DRAIIMARK(name)=Servlet_${TICKET}_ ## name" $CPLUSPLUSFLAGS  $LINKFLAGS -o "$SOFILE" "$CFILE"

exit $?
