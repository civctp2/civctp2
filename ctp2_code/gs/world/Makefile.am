#
# $Id$
#

include $(ctp2_code)/os/autoconf/Makefile.common

noinst_LTLIBRARIES = libgsworld.la

if WIN32
CTP2_WIN32_FREETYPE_INC=\
	-I$(ctp2_code)/ui/freetype
CTP2_WIN32_MILES_INC=\
	-I$(ctp2_code)/libs/miles
CTP2_WIN32_ZLIB_INC=\
	-I$(ctp2_code)/libs/zlib
endif
CTP2_ANET_INC=-I$(ctp2_code)/libs/anet/h

libgsworld_la_LIBADD = \
	../outcom/libgsoutcom.la

libgsworld_la_SOURCES = \
	Cell.cpp \
	cellunitlist.cpp \
	MapPoint.cpp \
	UnseenCell.cpp \
	wldgen.cpp \
	WorldDistance.cpp \
	worldevent.cpp \
	WrldCity.cpp \
	WrldCont.cpp \
	WrldPoll.cpp \
	WrlEnv.cpp \
	WrlImprove.cpp \
	WrlUnit.cpp

libgsworld_la_LDFLAGS = -static

AM_CFLAGS=\
	$(CTP2_CFLAGS)

AM_CXXFLAGS=\
	$(CTP2_CXXFLAGS)

AM_CPPFLAGS=\
	$(CTP2_CPPFLAGS)

