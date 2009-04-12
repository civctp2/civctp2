#
# $Id: Makefile.am 524 2006-01-16 21:18:46Z ctplinuxfan $
#

include $(ctp2_code)/os/autoconf/Makefile.common

noinst_LTLIBRARIES = libgsdatabase.la

if WIN32
CTP2_WIN32_FREETYPE_INC=\
	-I$(ctp2_code)/ui/freetype
CTP2_WIN32_MILES_INC=\
	-I$(ctp2_code)/libs/miles
CTP2_WIN32_ZLIB_INC=\
	-I$(ctp2_code)/libs/zlib
endif
CTP2_ANET_INC=-I$(ctp2_code)/libs/anet/h

# Files not in civctp.dsp:
# EndGameDB.cpp
# EndGameRecord.cpp
libgsdatabase_la_SOURCES = \
	DB.cpp \
	filenamedb.cpp \
	highscoredb.cpp \
	moviedb.cpp \
	PlayListDB.cpp \
	profileDB.cpp \
	Rec.cpp \
	StrDB.cpp \
	StrRec.cpp \
	thronedb.cpp \
	UVDB.cpp \
	UVRecord.cpp

libgsdatabase_la_LDFLAGS = -static

AM_CFLAGS=\
	$(CTP2_CFLAGS)

AM_CXXFLAGS=\
	$(CTP2_CXXFLAGS)

AM_CPPFLAGS=\
	$(CTP2_CPPFLAGS)

