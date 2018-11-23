# Makefile for client example program.
# Requires Watcom C386.  Run with "wmake".

# Set this to the directory containing the dp library and include files.
DPLIB= ..\..\dos\lib
DPINCLUDE= ..\..\h

.c.obj:
	wcc386 /mf /bt=dos /5r /dWCCREG /s /d2 /otexan /wx /zq /i=$(DPINCLUDE) /dDPRNT $*
	
# Link in the Greenleaf comm library (Greenleaf Software Inc., 214-248-2561)
GREENLEAF_LIB = $(DPLIB)\gcl510\gclfr32.lib
	
#--------------------------------------------------------------
# client.exe is client.c linked dynamically to the real comm layer.

client_LIB_FILES = $(DPLIB)\dp.lib $(GREENLEAF_LIB)

client_OBJ_FILES =	client.obj

# Set target dependencies to generate necessary re-compiles.
client.exe : .SYMBOLIC $(client_OBJ_FILES) client.lnk
  echo .
  echo Linking client executable.
  wlink $(WLINK_OPTIONS) @client.lnk

# Generate a new link file if necessary.
client.lnk : makefile
  echo .
  echo Preparing link file.
  %create $^@
  # Define exe name
  %append $^@ NAME  $^&
  # Set stack size
  %append $^@ OPTION STACK=16k
  # Define objects and libraries in exe.
  for %i in ($(client_OBJ_FILES)) do %append $^@ FILE %i
  #%append $^@ LIBPATH $(LIBDIR)
  for %i in ($(client_LIB_FILES)) do %append $^@ LIB  %i

clean: .SYMBOLIC
	del *.err
	del *.obj
	del *.exe
	del *.lnk
	del *.log
	del *.dll
	del *.lib
	del *.mdp
	del *.ncb
	
# end of MAKEFILE

