dnl
dnl $Id$
dnl

AC_DEFUN([AC_LIBANET_INTERNAL],[
   AC_REQUIRE([AC_CANONICAL_HOST])

   case "${host}" in
      *-linux*)
         AC_DEFINE(cdecl,,[cdecl dummy declaration])
	 AC_DEFINE(stricmp,strcasecmp,[function for case insensitive string comparison])
	 AC_DEFINE(strnicmp,strncasecmp,[function for case insensitive comparison of first n characters of a string])
	 AC_DEFINE_UNQUOTED(PACK,__attribute__ ((packed)),[compiler dependend instruction for packed attributes])
         AC_DEFINE(UNIX,1,[Define to 1 if you are compiling on Unix])
dnl      Anonymous unions must be disabled via AM_CPPFLAGS
dnl         AC_DEFINE(__NO_ANONYMOUS_UNIONS__,1,[Define to 1 if anonymous unions should be disabled])

dnl	 AC_MSG_CHECKING([for location of libbsd.a])
dnl         for libdir in $LD_LIBRARY_PATH /lib /usr/lib /usr/local/lib ; do
dnl            if test -f $libdir/libbsd.a; then
dnl               LIBBSD_COMPAT=$libdir/libbsd.a
dnl               break
dnl            fi
dnl            if test -f $libdir/libbsd-compat.a; then
dnl               LIBBSD_COMPAT=$libdir/libbsd-compat.a
dnl               break
dnl            fi
dnl         done
dnl         if test -n "$LIBBSD_COMPAT"; then
dnl            AC_MSG_RESULT([$LIBBSD_COMPAT])
dnl         else
dnl            AC_MSG_ERROR([not found])
dnl         fi

         ac_OpSystem="LINUX"
	 ac_OpClass="UNIX"
   esac

   ANET_CFLAGS_NORMAL="-DNDEBUG -O3"
   ANET_CFLAGS_DEBUG="-DDEBUG -g"
   ANET_CFLAGS_PROFILE="-DNDEBUG -D_PROFILE -O3 -pg"
   ANET_CFLAGS_LOG="-DNDEBUG -DPRNT -O3"

   AM_CONDITIONAL(UNIX, test x$ac_OpClass = xUNIX)
   AM_CONDITIONAL(LINUX, test x$ac_OpSystem = xLINUX)
   AM_CONDITIONAL(ANET_BUILD_DEBUG, test x = x)
   AM_CONDITIONAL(ANET_BUILD_PROFILING, test x = x)
   AM_CONDITIONAL(ANET_BUILD_LOG, test x = x)
   AC_SUBST(ANET_CFLAGS_NORMAL)
   AC_SUBST(ANET_CFLAGS_DEBUG)
   AC_SUBST(ANET_CFLAGS_PROFILE)
   AC_SUBST(ANET_CFLAGS_LOG)
   AC_SUBST(LIBBSD_COMPAT)
])

