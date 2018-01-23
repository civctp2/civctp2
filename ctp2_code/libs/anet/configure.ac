dnl $Id: $

AC_INIT([anet], [0.11])

AC_CONFIG_HEADERS([h/anet_config.h])
AC_CONFIG_AUX_DIR([macros])
AC_CONFIG_MACRO_DIR([macros])
AC_CONFIG_FILES([GNUmakefile])

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE(foreign subdir-objects)
#AM_SILENT_RULES([yes])
AC_PROG_MAKE_SET

AC_PROG_CC(cl gcc cc KCC CC egcs)
LT_INIT
AC_LANG(C)
AC_PATH_PROG(UNZIP,unzip,no)
if test "x$UNZIP" == "xno"
then
   AC_MSG_ERROR([

   unzip could not be found!

	Make sure unzip is in your path.

])
fi
AC_SUBST(UNZIP)

AC_LIBANET_INTERNAL

AC_OUTPUT
