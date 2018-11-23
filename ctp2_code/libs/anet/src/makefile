# Makefile for the ActiveNet SDK
# $Log: makefile $
# Revision 1.26  1997/09/28 19:48:54  dkegel
# Removed turn from demo, since it doesn't work with dp2 yet
# Revision 1.25  1997/09/01 19:23:34  dkegel
# Clean up test, too.
# Revision 1.24  1997/07/17 22:17:29  lbennett
# wmake under NT needs .bat on end of batch files!
# Revision 1.23  1997/07/16 22:58:34  dkegel
# Works.
# Avoids storing jre in src zip.
# Revision 1.22  1997/07/16 21:06:28  dkegel
# Added ../demo.zip, holding netshell, chat, and turn
# Revision 1.21  1997/07/16 01:26:59  dkegel
# Let netshell clean up after itself.
# Revision 1.20  1997/06/27 17:07:15  dkegel
# Include *contents* of doc and h directories in anetsrc.zip!
# Revision 1.19  1997/06/23 23:50:09  dkegel
# fix how zip is used - avoid wildcards if doing whole dir
# Revision 1.18  1997/06/23 03:46:28  dkegel
# Recursing with zip (instead of pkzip) means ya gotta use *, not *.*,
# to match names that lack embedded .'s.
# Revision 1.17  1997/06/23 03:17:50  dkegel
# Switched to info-zip's zip for the sake of long file names
# Revision 1.16  1997/06/23 02:13:08  dkegel
# Added java.
# Revision 1.15  1997/06/05 00:49:33  dkegel
# SDK needs a few more .h files
# Revision 1.14  1997/06/02 04:09:00  dkegel
# Add .sit files, if they're there.
# Revision 1.13  1997/03/28 20:14:19  dkegel
# Split sdk into sdk and src zip files.
# Revision 1.12  1997/03/02 00:45:37  dkegel
# Include bin.
# Revision 1.11  1997/03/01 22:11:35  dkegel
# Don't prompt during clean.
# Revision 1.10  1997/02/07 02:34:04  dkegel
# Works with new dos/dp directory.
# Revision 1.9  1997/02/06 22:58:46  dkegel
# The sdk users need source!
# Revision 1.8  1997/02/02 06:39:03  dkegel
# Renamed sdk.zip to anetsdk.zip
# Revision 1.7  1997/02/02 04:16:43  dkegel
# Be more explicit about which files to add to sdk.zip
# Revision 1.6  1997/02/01 02:06:36  dkegel
# 1. Don't include .c files.
# 2. Call demo's makefile to clean.
# Revision 1.5  1996/12/13 16:10:16  dkegel
# Better clean for dp.
# Revision 1.4  1996/12/13 04:08:16  dkegel
# Moved code to build everything into build.bat to get around RAM problem

clean: .SYMBOLIC
	cd score
		clean.bat
	cd ..
	cd tca
		clean.bat
	cd ..
	cd win
		clean.bat
	cd ..
	cd java
		nmake clean
	cd ..
	cd ..\demo
		clean.bat
	cd ..\src
	cd test
		clean.bat
	cd ..
	cd aeh
		clean.bat
	cd ..

# Also clean stuff dumped into the parent directory.
ultraclean: .SYMBOLIC
	cd ..
	deltree /y dos\lib win\dll win\lib
	cd demo
	wmake clean
	cd ..\src
