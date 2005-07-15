#
# $Id$
#
noinst_PROGRAMS=ctpdb
ctpdb_SOURCES=\
	Datum.cpp \
	MemberClass.cpp \
	RecordDescription.cpp \
	ctpdb.cpp \
	ydb.tab.c \
	lex.yydb.c

AM_CPPFLAGS=\
	$(CTP2_NOWIN32_INC) \
	-I$(ctp2_code)/os/include \
	-I$(ctp2_code)/ctp/ctp2_utils

ydb.tab.c ydb.tab.h: ctpdb.y
	$(YACC) $(YFLAGS) $(AM_YFLAGS) -p yydb -b ydb $<

lex.yydb.c: ctpdb.l ydb.tab.h
	$(LEX) -i -Pyydb -o$@ $<

MOSTLYCLEANFILES=\
	ctpdb_lex.c \
	ctpdb_yacc.c \
	ctpdb_yacc.h \
	lex.yydb.c \
	ydb.output \
	ydb.tab.c \
	ydb.tab.h
