#
# $Id$
#
noinst_PROGRAMS=ctpdb
ctpdb_SOURCES=\
	Datum.cpp \
	MemberClass.cpp \
	RecordDescription.cpp \
	ctpdb.cpp \
	y.tab.c \
	lex.yy.c

AM_CPPFLAGS=\
	$(CTP2_NOWIN32_INC) \
	-I$(ctp2_code)/os/include \
	-I$(ctp2_code)/ctp/ctp2_utils

y.tab.c: ctpdb.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) $<

y.tab.h: ctpdb.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) $<

lex.yy.c: ctpdb.l y.tab.h
	$(LEX) -i $<

MOSTLYCLEANFILES=\
	ctpdb_lex.c \
	ctpdb_yacc.c \
	ctpdb_yacc.h \
	lex.yy.c \
	y.output \
	y.tab.c \
	y.tab.h
