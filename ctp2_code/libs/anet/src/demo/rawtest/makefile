# Makefile for raw packet example program.
# Requires Watcom C386.  Run with "wmake".

# Set this to the directory containing the dp library and include files.
DPLIB=..\..\dos\lib
DPINCLUDE=..\..\h

.c.obj:
	wcc386 /mf /bt=dos /5r /dWCCREG /s /d2 /otexan /wx /zq /i=$(DPINCLUDE);..\netsnake /dDPRNT $*
	
# Link in the Greenleaf comm library (Greenleaf Software Inc., 214-248-2561)
GREENLEAF_LIB = $(DPLIB)\gcl510\gclfr32.lib
	
default: rawtest.exe

#--------------------------------------------------------------
# rawtest.exe is rawtest.c linked dynamically to the real comm layer.

rawtest_LIB_FILES = $(DPLIB)\dp.lib $(GREENLEAF_LIB)

rawtest_OBJ_FILES =	rawtest.obj dprint.obj

dprint.c: ..\..\src\dp\dprint.c
	copy ..\..\src\dp\dprint.c
	
# Set target dependencies to generate necessary re-compiles.
rawtest.exe : dprint.c $(rawtest_OBJ_FILES) rawtest.lnk
  echo .
  echo Linking rawtest executable.
  wlink $(WLINK_OPTIONS) @rawtest.lnk

# Generate a new link file if necessary.
rawtest.lnk : makefile
  echo .
  echo Preparing link file.
  %create $^@
  # Define exe name
  %append $^@ NAME  $^&
  # Define objects and libraries in exe.
  for %i in ($(rawtest_OBJ_FILES)) do %append $^@ FILE %i
  #%append $^@ LIBPATH $(LIBDIR)
  for %i in ($(rawtest_LIB_FILES)) do %append $^@ LIB  %i

clean: .SYMBOLIC
	del dprint.?
	del *.err
	del *.obj
	del *.exe
	del *.lnk
	del *.log
	del *.dll
	del *.lib
	
# end of MAKEFILE

