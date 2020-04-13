dnl
dnl $Id$
dnl

AC_DEFUN([AM_PATH_SDL_FFMPEG],[
   AC_CHECK_HEADER([libavcodec/avcodec.h],[av_codec_found_header=yes],[av_codec_found_header=no])
   AC_CHECK_HEADER([libavformat/avformat.h],[av_format_found_header=yes],[av_format_found_header=no])
   AC_CHECK_HEADER([libswscale/swscale.h],[sw_scale_found_header=yes],[sw_scale_found_header=no])
   if test "x$av_codec_found_header" = xyes -a "x$av_format_found_header" = xyes -a "x$sw_scale_found_header" = xyes; then
       SDL_FFMPEG_LDFLAGS="-lavutil -lavformat -lavcodec -lswscale -lswresample"
       AC_DEFINE(USE_SDL_FFMPEG,1,[Enable SDL_ffmpeg library])
   else
       SDL_FFMPEG_LDFLAGS=""
   fi

   AC_SUBST(SDL_FFMPEG_LDFLAGS)
])
