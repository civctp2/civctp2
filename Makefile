# This Makefile is intended to automate certain useful tasks
# for developers.
# It is NOT intended to be used to build the CTP2 executable
#
# Provided targets are:
# local
#   Will copy relevant files from the source tree to a local installation,
#   to allow them to be tested there.
#   The location of the local installation is taken from etc/local-install
#   (Note that is a local etc directory, not /etc).
#   This target uses a shell script, so it will require, for example, cygwin
#   to work.
#
# playtest
#   Will create a directory called playtest and copy into it all appropriate
#   files to constitute a playtest version, ready to be compressed and
#   distributed.  This starts by deleting the playtest directoy, so
#   be sure there is nothing there you wish to preserve!
#
# doc
#   Make all the documentation

all:
	#################################### \
	# This Makefile has no default     # \
	# or all target.                   # \
	# See the comments in the Makefile # \
	# for a description of its use.    # \
	####################################

local:
	bin/copy_to_local.sh

PLAYTEST = playtest

playtest:
	rm -rf playtest
	mkdir -p $(PLAYTEST)/ctp2_program/ctp/dll/
	svn export ctp2_code/ctp/dll $(PLAYTEST)/ctp2_program/ctp/dll --force
	cp -uv ctp2_code/ctp/dll/map/*.dll $(PLAYTEST)/ctp2_program/ctp/dll/map/
	cp -uv ctp2_code/ctp/anet* $(PLAYTEST)/ctp2_program/ctp/
	cp -uv ctp2_code/ctp/ctp2.* $(PLAYTEST)/ctp2_program/ctp/
	cp -uv ctp2_code/ctp/appstr.txt $(PLAYTEST)/ctp2_program/ctp/
	svn export ctp2_data $(PLAYTEST)/ctp2_data --force
	svn export Scenarios $(PLAYTEST)/Scenarios --force
# TODO: The playtest should include the user documentation too

doc:
	$(MAKE) -C doc

.PHONY: all local playtest doc
