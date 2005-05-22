topdir = ../../

all: ChangeLog.txt

ChangeLog.txt: .svn/entries
	svn log $(topdir) > $@

.PHONY: all
