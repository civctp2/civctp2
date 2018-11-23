topdir = ../../../
thisdir = doc/user/playtest/

all: ChangeLog.txt

ChangeLog.txt: .svn/entries
	cd $(topdir) && svn log > $(thisdir)$@

.PHONY: all

.DELETE_ON_ERROR:

