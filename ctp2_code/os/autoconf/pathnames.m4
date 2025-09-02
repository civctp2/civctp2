dnl
dnl $Id$
dnl

AC_DEFUN([AC_OS_PATHNAMES],[
   AC_REQUIRE([AC_CANONICAL_HOST])

   case "${host}" in
      *-cygwin*)
         AC_DEFINE_UNQUOTED(FILE_SEP,"\\\\",[File separator used on windows])
         AC_DEFINE_UNQUOTED(FILE_SEPC,'\\\\',[File separator used on windows])
         AC_DEFINE_UNQUOTED(PATH_SEP,";",[Path separator used on windows])
         AC_DEFINE_UNQUOTED(PATH_SEPC,';',[Path separator used on windows])
         ;;
      *)
         AC_DEFINE_UNQUOTED(FILE_SEP,"/",[File separator on any other os])
         AC_DEFINE_UNQUOTED(FILE_SEPC,'/',[File separator on any other os])
         AC_DEFINE(MAX_PATH,PATH_MAX,[Maximum length of pathnames])
         AC_DEFINE(_MAX_PATH,260,[Maximum length of a path, in bytes. It must be 260, otherwise you cannot load savegames from Windows.])
         AC_DEFINE_UNQUOTED(PATH_SEP,":",[Path separator on any other os])
         AC_DEFINE_UNQUOTED(PATH_SEPC,':',[Path separator on any other os])
   esac
])
