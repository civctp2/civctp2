dnl
dnl $Id$
dnl

AC_DEFUN([AC_OS_DEFINES],[
   AC_REQUIRE([AC_CANONICAL_HOST])

   case "${host}" in
      *-cygwin*)
         AC_DEFINE(WIN32,1,[Define to 1 if you are compiling on Windows])
         AC_DEFINE(_WINDOWS,1,[Define to 1 if you are compiling on Windows])
         ac_OpSystem="WIN32"
         ;;
      *-linux*)
         AC_DEFINE(LINUX,1,[Define to 1 if you are compiling on Linux])
         ac_OpSystem="LINUX"
   esac

   AM_CONDITIONAL(WIN32, test x$ac_OpSystem = xWIN32)
   AM_CONDITIONAL(LINUX, test x$ac_OpSystem = xLINUX)
])
