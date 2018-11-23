dnl
dnl $Id$
dnl

AC_DEFUN([AM_PATH_SDL_FFMPEG],[
   AC_CHECK_HEADER([SDL/SDL_ffmpeg.h],[ac_found_header=yes],[ac_found_header=no])
   if test x$ac_found_header = xyes; then
       SDL_FFMPEG_LDFLAGS="-lSDL_ffmpeg -lavcodec"
       AC_DEFINE(USE_SDL_FFMPEG,1,[Enable SDL_ffmpeg library])
   else
       SDL_FFMPEG_LDFLAGS=""
   fi

   AC_SUBST(SDL_FFMPEG_LDFLAGS)
])
