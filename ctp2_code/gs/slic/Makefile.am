#
# $Id: Makefile.am 464 2005-09-19 21:34:50Z john $
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

libgsslic_la_LDFLAGS = -static

libgsslic_la_SOURCES = \
	lex.yysc.c \
	lex.yysl.c \
	ysc.tab.c \
	ysc.tab.h \
	ysl.tab.c \
	ysl.tab.h \
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

lex.yysc.c: sliccmd.l ysc.tab.h
	$(LEX) -i -Pyysc -o$@ $<

lex.yysl.c: slic.l ysl.tab.h
	$(LEX) -i -Pyysl -o$@ $<

ysc.tab.c \
ysc.tab.h: sliccmd.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p yysc -bysc $<

ysl.tab.c \
ysl.tab.h: slic.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p yysl -bysl $<

MOSTLYCLEANFILES = \
	lex.yysc.c \
	lex.yysl.c \
	ysc.tab.c \
	ysc.tab.h \
	ysl.tab.c \
	ysl.tab.h \
	ysl.output \
	ysc.output

AM_CXXFLAGS=\
	$(SDL_CFLAGS)

AM_CPPFLAGS=\
	$(CTP2_CPPFLAGS)

