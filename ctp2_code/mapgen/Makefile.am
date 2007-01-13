#
# $Id$
#
plugindir = $(libdir)/ctp2
plugin_LTLIBRARIES=crater.la fault.la geometric.la plasma2.la

if WIN32
CTP2_WIN32_FREETYPE_INC=\
	-I$(ctp2_code)/ui/freetype
endif

crater_la_SOURCES = Crater.cpp
crater_la_LDFLAGS = -module -avoid-version

fault_la_SOURCES = FaultGen.cpp
fault_la_LDFLAGS = -module -avoid-version

geometric_la_SOURCES = Geometric.cpp
geometric_la_LDFLAGS = -module -avoid-version

plasma2_la_SOURCES = PlasmaGen2.cpp
plasma2_la_LDFLAGS = -module -avoid-version

AM_CPPFLAGS = \
	$(DIRECTX_INCLUDES) \
	$(DXMEDIA_INCLUDES) \
	$(CTP2_WIN32_FREETYPE_INC) \
	$(CTP2_NOWIN32_INC) \
	-I$(ctp2_code)/os/include \
	-I$(ctp2_code)/ctp \
	-I$(ctp2_code)/ctp/ctp2_utils \
	-I$(ctp2_code)/gs/outcom \
	-I$(ctp2_code)/ui/aui_common \
	-I$(ctp2_code)/ui/ldl

