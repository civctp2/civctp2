#
# $Id$
#

bin_PROGRAMS = ctp2
# This hack shortens include path list,
# but prohibits using a separate build dir
ctp2_code=..

ctp2_SOURCES = \
	ctp2_utils/appstrings.cpp \
	ctp2_utils/c3cmdline.cpp \
	ctp2_utils/c3cpu.cpp \
	ctp2_utils/c3debug.cpp \
	ctp2_utils/c3errors.cpp \
	ctp2_utils/c3files.cpp \
	ctp2_utils/c3mem.cpp \
	ctp2_utils/cheatkey.cpp \
	ctp2_utils/ctpregistry.cpp \
	ctp2_utils/ddhelp.cpp \
	ctp2_utils/netconsole.cpp \
	ctp2_utils/tracklen.cpp

AM_CPPFLAGS=\
	-I$(ctp2_code)/os/include \
	-I$(ctp2_code)/ctp \
	-I$(ctp2_code)/ctp/ctp2_utils \
		$(INCORDER_ctp2_code__ctp__ctp2_rsrc) \
	-I$(ctp2_code)/ctp/debugtools \
		$(INCORDER_ctp2_code__libs__freetype__lib) \
		$(INCORDER_ctp2_code__libs__freetype__utils) \
		$(INCORDER_ctp2_code__libs__freetype__win32) \
	-I$(ctp2_code)/ui/interface \
	-I$(ctp2_code)/ui/netshell \
	-I$(ctp2_code)/robot/utility \
	-I$(ctp2_code)/robot/pathing \
	-I$(ctp2_code)/robot/aibackdoor \
		$(INCORDER_ctp2_code__libs__freetype) \
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
		$(INCORDER_ctp2_code__libs__anet__h) \
	-I$(ctp2_code)/mm \
	-I$(ctp2_code)/robotcom/backdoor \
	-I$(ctp2_code)/gs/slic \
	-I$(ctp2_code)/gfx/layers \
	-I$(ctp2_code)/mapgen \
	-I$(ctp2_code)/ui/freetype \
	-I$(ctp2_code)/sound \
	-I$(ctp2_code)/GameWatch/gamewatch \
	-I$(ctp2_code)/GameWatch/gwciv \
		-I$(INCORDER_ctp2_code__libs__miles) \
	-I$(ctp2_code)/ctp/fingerprint \
		$(INCORDER_ctp2_code__libs__zlib) \
	-I$(ctp2_code)/ui/slic_debug \
	-I$(ctp2_code)/gs/outcom \
	-I$(ctp2_code)/ctp \
	-I$(ctp2_code)/ui/aui_common \
	-I$(ctp2_code)/ui/ldl \
		$(INCORDER_ctp2_code__libs__dxmedia__classes__base) \
		$(INCORDER_ctp2_code__libs__dxmedia__include) \
		$(INCORDER_ctp2_code__libs__directx__include) \
	-I$(ctp2_code)/gs/events \
	-I$(ctp2_code)/gs/newdb \
	-I$(ctp2_code)/ai/diplomacy \
	-I$(ctp2_code)/ai/mapanalysis \
	-I$(ctp2_code)/ai/strategy/scheduler \
	-I$(ctp2_code)/ai/strategy/agents \
	-I$(ctp2_code)/ai/strategy/goals \
	-I$(ctp2_code)/ai/strategy/squads \
	-I$(ctp2_code)/ai/citymanagement


