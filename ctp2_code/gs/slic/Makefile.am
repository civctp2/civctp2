#
# $Id$
#

include $(ctp2_code)/os/autoconf/Makefile.common

noinst_LTLIBRARIES = libgsslic.la

if WIN32
CTP2_WIN32_FREETYPE_INC=\
	-I$(ctp2_code)/ui/freetype
CTP2_WIN32_MILES_INC=\
	-I$(ctp2_code)/libs/miles
CTP2_WIN32_ZLIB_INC=\
	-I$(ctp2_code)/libs/zlib
endif
CTP2_ANET_INC=-I$(ctp2_code)/libs/anet/h

libgsslic_la_SOURCES = \
	lex.sc.c \
	lex.yy.c \
	sc.tab.c \
	sc.tab.h \
	y.tab.c \
	y.tab.h \
	QuickSlic.cpp \
	SlicArray.cpp \
	SlicBuiltin.cpp \
	SlicButton.cpp \
	sliccmd.cpp \
	SlicConditional.cpp \
	SlicConst.cpp \
	SlicContext.cpp \
	SlicEngine.cpp \
	SlicEyePoint.cpp \
	SlicFrame.cpp \
	slicfunc.cpp \
	slicfuncai.cpp \
	slicif.cpp \
	sliciffile.cpp \
	SlicModFunction.cpp \
	SlicNamedSymbol.cpp \
	slicobject.cpp \
	SlicRecord.cpp \
	SlicSegment.cpp \
	SlicStack.cpp \
	SlicStruct.cpp \
	SlicSymbol.cpp \
	SlicSymTab.cpp

libgsslic_la_LDFLAGS = -static

lex.sc.c: sliccmd.l sc.tab.h
	$(LEX) -i -o$@ $<

lex.yy.c: slic.l y.tab.h
	$(LEX) -i -o$@ $<

sc.tab.c \
sc.tab.h: sliccmd.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -bsc $<

y.tab.c \
y.tab.h: slic.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -by $<

MOSTLYCLEANFILES = \
	lex.sc.c \
	lex.yy.c \
	sc.tab.c \
	sc.tab.h \
	y.tab.c \
	y.tab.h \
	y.output

AM_CXXFLAGS=\
	$(SDL_CFLAGS)

AM_CPPFLAGS=\
	$(CTP2_CPPFLAGS)

