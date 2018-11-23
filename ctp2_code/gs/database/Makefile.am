#
# $Id$
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

libgsdatabase_la_SOURCES = \
	CivilisationDB.cpp \
	CivilisationRec.cpp \
	conceptdb.cpp \
	ConstDB.cpp \
	DB.cpp \
	DiffDB.cpp \
	filenamedb.cpp \
	gwdb.cpp \
	GWRecord.cpp \
	highscoredb.cpp \
	MapDB.cpp \
	MapRecord.cpp \
	moviedb.cpp \
	orderdb.cpp \
	PlayListDB.cpp \
	pollutiondb.cpp \
	PollutionRecord.cpp \
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

