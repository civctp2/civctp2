#
# $Id$
#

include $(ctp2_code)/os/autoconf/Makefile.common

noinst_LTLIBRARIES = libgfxspritesys.la

if WIN32
CTP2_WIN32_FREETYPE_INC=\
	-I$(ctp2_code)/ui/freetype
CTP2_WIN32_MILES_INC=\
	-I$(ctp2_code)/libs/miles
CTP2_WIN32_ZLIB_INC=\
	-I$(ctp2_code)/libs/zlib
endif
CTP2_ANET_INC=-I$(ctp2_code)/libs/anet/h

libgfxspritesys_la_SOURCES = \
	action.cpp \
	Actor.cpp \
	ActorPath.cpp \
	Anim.cpp \
	battleviewactor.cpp \
	EffectActor.cpp \
	goodactor.cpp \
	TradeActor.cpp \
	UnitActor.cpp \
	workeractor.cpp \
	CitySpriteGroup.cpp \
	effectspritegroup.cpp \
	FacedSprite.cpp \
	FacedSpriteWshadow.cpp \
	goodspritegroup.cpp \
	Sprite.cpp \
	spritefile.cpp \
	SpriteGroup.cpp \
	SpriteGroupList.cpp \
	SpriteHitTest.cpp \
	SpriteList.cpp \
	SpriteLow555.cpp \
	SpriteLow565.cpp \
	SpriteState.cpp \
	spriteutils.cpp \
	UnitSpriteGroup.cpp \
	director.cpp \
	screenmanager.cpp
# directorevent.cpp removed to top level Makefile.am due to bizarre linking problems

libgfxspritesys_la_LDFLAGS = -static

AM_CFLAGS=\
	$(CTP2_CFLAGS)

AM_CXXFLAGS=\
	$(CTP2_CXXFLAGS)

AM_CPPFLAGS=\
	$(CTP2_CPPFLAGS)

