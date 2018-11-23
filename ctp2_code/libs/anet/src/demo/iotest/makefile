# Makefile for raw packet example program.
# Requires Watcom C386.  Run with "wmake".
# $Log: makefile $
# Revision 1.1  1997/02/08 01:02:31  dkegel
# Initial revision

# Set this to the directory containing the dp library and include files.
DPLIB=..\..\dos\lib
DPINCLUDE=..\..\h

.c.obj:
	wcc386 /mf /bt=dos /5r /dWCCREG /s /d2 /otexan /wx /zq /i=$(DPINCLUDE);..\netsnake /dDPRNT $*
	
# Link in the Greenleaf comm library (Greenleaf Software Inc., 214-248-2561)
GREENLEAF_LIB = $(DPLIB)\gcl510\gclfr32.lib
	
default: iotest.exe

#--------------------------------------------------------------
# iotest.exe is iotest.c linked dynamically to the real comm layer.

iotest_LIB_FILES = $(DPLIB)\dp.lib $(GREENLEAF_LIB)

iotest_OBJ_FILES =	iotest.obj 

# Set target dependencies to generate necessary re-compiles.
iotest.exe : obj $(iotest_OBJ_FILES) iotest.lnk
  echo .
  echo Linking iotest executable.
  wlink $(WLINK_OPTIONS) @iotest.lnk

# Generate a new link file if necessary.
iotest.lnk : makefile
  echo .
  echo Preparing link file.
  %create $^@
  # Define exe name
  %append $^@ NAME  $^&
  # Define objects and libraries in exe.
  for %i in ($(iotest_OBJ_FILES)) do %append $^@ FILE %i
  #%append $^@ LIBPATH $(LIBDIR)
  for %i in ($(iotest_LIB_FILES)) do %append $^@ LIB  %i

obj: .SYMBOLIC
	-mkdir obj
	
clean: .SYMBOLIC
	del *.err
	del *.obj
	del *.exe
	del *.lnk
	del *.log
	del *.dll
	del *.lib
	
# end of MAKEFILE

