dnl
dnl $Id$
dnl

AC_DEFUN([AC_OS_DEFINES],[
   AC_REQUIRE([AC_CANONICAL_HOST])

   case "${host}" in
      *-cygwin*)
         AC_DEFINE(WIN32,1,[Define to 1 if you are compiling on Windows])
         AC_DEFINE(_WINDOWS,1,[Define to 1 if you are compiling on Windows])
         ;;
      *-linux*)
         AC_DEFINE(LINUX,1,[Define to 1 if you are compiling on Linux])
   esac
])
