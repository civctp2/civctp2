dnl $Id$
dnl
AC_INIT(ctp2,1.2)
AC_PREREQ(2.54)

AC_CONFIG_AUX_DIR(ctp2_code/os/autoconf/config)
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(ctp2_code/os/include/config.h)
AM_MAINTAINER_MODE
AC_PROG_MAKE_SET

AC_PROG_CC(cl gcc cc KCC CC egcs)
AC_PROG_CXX(cl g++ gcc cxx c++ cc++ KCC CC egcs)
AC_LANG(C++)
AM_PROG_LEX
dnl AC_PROG_YACC
AC_PATH_PROG(YACC,byacc)
if test "x$YACC" == "xno"
then
   AC_MSG_ERROR([

        byacc could not be found!

        Make sure byacc is in your path.
 
   ])
else
   YACC="${YACC}"
   YFLAGS="-d -v -t"
   AC_SUBST(YACC)
   AC_SUBST(YFLAGS)
   AC_DEFINE(AUTOMAKE_INVOKES_YACC,1,[Defined when automake runs yacc and renames the output files])
fi

AC_PROG_LIBTOOL

AM_PATH_DIRECTX
AM_PATH_DXMEDIA
AM_PATH_SDL
AC_DEFINE(USE_SDL,1,[Use the SDL-library])

AC_OS_DEFINES
AC_OS_PATHNAMES

AC_CHECK_HEADERS(sys/param.h)
case $host in
*-cygwin*)
	;;
*)	AC_DEFINE(USE_COM_REPLACEMENT,1,[Do not use COM])
	;;
esac
AC_DEFINE(YYDEBUG,9,[Debug Parsers])

for ac_remove_CFLAG in "-O1" "-O2" "-O3" ; do
	CFLAGS=${CFLAGS//${ac_remove_CFLAG}/}
	CPPFLAGS=${CPPFLAGS//${ac_remove_CFLAG}/}
	CXXFLAGS=${CXXFLAGS//${ac_remove_CFLAG}/}
done

ctp2_code='$(top_srcdir)/ctp2_code'
AC_SUBST(ctp2_code)

CFLAGS="$CFLAGS"
CXXFLAGS="-fms-extensions $CXXFLAGS"
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(CXXFLAGS)

AC_OUTPUT([
   GNUmakefile
   ctp2_code/Makefile
   ctp2_code/ctp/Makefile
   ctp2_code/gs/Makefile
   ctp2_code/gs/dbgen/Makefile
   ctp2_code/gs/newdb/Makefile
   ctp2_code/mapgen/Makefile
])
