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

	 AC_MSG_CHECKING([for location of libbsd.a])
         for libdir in $LD_LIBRARY_PATH /lib /usr/lib /usr/local/lib ; do
            if test -f $libdir/libbsd.a; then
               LIBBSD_COMPAT=$libdir/libbsd.a
               break
            fi
            if test -f $libdir/libbsd-compat.a; then
               LIBBSD_COMPAT=$libdir/libbsd-compat.a
               break
            fi
         done
         if test -n "$LIBBSD_COMPAT"; then
            AC_MSG_RESULT([$LIBBSD_COMPAT])
         else
            AC_MSG_ERROR([not found])
         fi
	 
         ac_OpSystem="LINUX"
	 ac_OpClass="UNIX"
   esac

   ANET_CFLAGS_NORMAL="-DNDEBUG -O3 -Werror"
   ANET_CFLAGS_DEBUG="-DDEBUG -g -Werror"
   ANET_CFLAGS_PROFILE="-DNDEBUG -D_PROFILE -O3 -pg -Werror"
   ANET_CFLAGS_LOG="-DNDEBUG -DPRNT -O3 -Werror"

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

