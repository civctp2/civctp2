#
# $Id$
#

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
	$(CTP2_NOWIN32_INC) \
	$(DIRECTX_INCLUDES) \
	$(DXMEDIA_INCLUDES) \
	-I$(ctp2_code)/os/include \
	-I$(ctp2_code)/ctp \
	-I$(ctp2_code)/ctp/ctp2_utils \
	-I$(ctp2_code)/ctp/ctp2_rsrc \
	$(CTP2_WIN32_RSRC_INC) \
	-I$(ctp2_code)/ctp/debugtools \
		$(INCORDER_ctp2_code__libs__freetype__lib) \
		$(INCORDER_ctp2_code__libs__freetype__utils) \
		$(INCORDER_ctp2_code__libs__freetype__win32) \
	-I$(ctp2_code)/ui/interface \
	-I$(ctp2_code)/ui/netshell \
	-I$(ctp2_code)/robot/utility \
	-I$(ctp2_code)/robot/pathing \
	-I$(ctp2_code)/robot/aibackdoor \
	$(CTP2_WIN32_FREETYPE_INC) \
	-I$(ctp2_code)/gfx/spritesys \
	-I$(ctp2_code)/gfx/tilesys \
	-I$(ctp2_code)/gfx/gfx_utils \
	-I$(ctp2_code)/gs/database \
	-I$(ctp2_code)/gs/fileio \
	-I$(ctp2_code)/gs/gameobj \
	-I$(ctp2_code)/gs/utility \
	-I$(ctp2_code)/gs/world \
	-I$(ctp2_code)/net/io \
	-I$(ctp2_code)/net/general \
	-I$(ctp2_code)/ui/aui_utils \
	-I$(ctp2_code)/ui/aui_sdl \
	-I$(ctp2_code)/ui/aui_directx \
	-I$(ctp2_code)/ui/aui_ctp2 \
	-I$(ctp2_code)/ui/aui_common \
		$(INCORDER_ctp2_code__libs__tiff) \
		$(CTP2_ANET_INC) \
	-I$(ctp2_code)/mm \
	-I$(ctp2_code)/robotcom/backdoor \
	-I$(top_builddir)/ctp2_code/gs/slic \
	-I$(ctp2_code)/gs/slic \
	-I$(ctp2_code)/gfx/layers \
	-I$(ctp2_code)/mapgen \
	-I$(ctp2_code)/ui/freetype \
	-I$(ctp2_code)/sound \
	-I$(ctp2_code)/GameWatch/gamewatch \
	-I$(ctp2_code)/GameWatch/gwciv \
	$(CTP2_WIN32_MILES_INC) \
	-I$(ctp2_code)/ctp/fingerprint \
	$(CTP2_WIN32_ZLIB_INC) \
	-I$(ctp2_code)/ui/slic_debug \
	-I$(ctp2_code)/gs/outcom \
	-I$(ctp2_code)/ctp \
	-I$(ctp2_code)/ui/aui_common \
	-I$(top_builddir)/ctp2_code/ui/ldl \
	-I$(ctp2_code)/ui/ldl \
		$(INCORDER_ctp2_code__libs__dxmedia__classes__base) \
		$(INCORDER_ctp2_code__libs__dxmedia__include) \
		$(INCORDER_ctp2_code__libs__directx__include) \
	-I$(ctp2_code)/gs/events \
	-I$(top_builddir)/ctp2_code/gs/newdb \
	-I$(ctp2_code)/gs/newdb \
	-I$(ctp2_code)/ai/diplomacy \
	-I$(ctp2_code)/ai/mapanalysis \
	-I$(ctp2_code)/ai/strategy/scheduler \
	-I$(ctp2_code)/ai/strategy/agents \
	-I$(ctp2_code)/ai/strategy/goals \
	-I$(ctp2_code)/ai/strategy/squads \
	-I$(ctp2_code)/ai \
	-I$(ctp2_code)/ai/citymanagement

