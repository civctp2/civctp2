

INCS=-I../../ctp/ctp2_utils

all: y.tab.c lex.yy.c ctpdb.cpp ctpdb.h RecordDescription.cpp RecordDescription.h MemberClass.h MemberClass.cpp Datum.h Datum.cpp
	gcc $(INCS) -o ctpdb y.tab.c lex.yy.c ctpdb.cpp RecordDescription.cpp MemberClass.cpp Datum.cpp

y.tab.c: ctpdb.y
	byacc -d ctpdb.y

y.tab.h: ctpdb.y
	byacc -d ctpdb.y

lex.yy.c: ctpdb.l y.tab.h
	flex -i ctpdb.l
