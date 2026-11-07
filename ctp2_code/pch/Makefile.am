#
# $Id$
#
# Precompiled header for ctp/c3.h, included (directly or transitively) by
# almost every source file in the project. Built first (this subdir is
# listed before all others in the top-level SUBDIRS) so every other
# directory's compiles can pick it up automatically.
#
# gcc supports multiple precompiled variants of the same header: if
# "c3.h.gch" is a directory (rather than a single .gch file), gcc tries
# every file inside it and silently falls back to a normal, uncached
# compile of c3.h if none of them were built with matching flags - so
# this is safe even where it doesn't apply.
#
# Two variants are needed because the project's own subdirectories
# compile with two different, incompatible flag sets:
#   - most SUBDIRS build a noinst_LTLIBRARIES convenience library, which
#     libtool compiles with -fPIC -DPIC added;
#   - the top-level bin_PROGRAMS' own sources (ai/, ctp/, ui/, ...) are
#     compiled directly, without libtool, and pick up this distro's
#     default -fpie instead - which gcc's pch validity check treats as
#     incompatible with a header precompiled with -fPIC.

include $(ctp2_code)/os/autoconf/Makefile.common

AM_CXXFLAGS=\
	$(CTP2_CXXFLAGS)

AM_CPPFLAGS=\
	$(CTP2_CPPFLAGS)

PCH_HEADER = $(ctp2_code)/ctp/c3.h
PCH_DIR = $(ctp2_code)/ctp/c3.h.gch
PCH_PLAIN = $(PCH_DIR)/plain.gch
PCH_PIC = $(PCH_DIR)/pic.gch

all-local: $(PCH_PLAIN) $(PCH_PIC)

$(PCH_PLAIN): $(PCH_HEADER)
	@$(MKDIR_P) $(PCH_DIR)
	@$(MKDIR_P) $(DEPDIR)
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) -x c++-header -MT $@ -MD -MP -MF $(DEPDIR)/plain.gch.Po -o $@ $<

$(PCH_PIC): $(PCH_HEADER)
	@$(MKDIR_P) $(PCH_DIR)
	@$(MKDIR_P) $(DEPDIR)
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS) -fPIC -DPIC -x c++-header -MT $@ -MD -MP -MF $(DEPDIR)/pic.gch.Po -o $@ $<

-include $(DEPDIR)/plain.gch.Po
-include $(DEPDIR)/pic.gch.Po

clean-local:
	rm -rf $(PCH_DIR)

DISTCLEANFILES = $(DEPDIR)/plain.gch.Po $(DEPDIR)/pic.gch.Po
