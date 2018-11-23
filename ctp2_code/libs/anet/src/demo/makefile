clean: .SYMBOLIC
    cd client
	wmake clean
	nmake -f client.mak clean
	cd ..
	
	cd netsnake
	wmake clean 
	cd ..
	
	cd rawtest
	wmake clean 
	nmake -f rawtest.mak clean
	cd ..
	
	for %a in (shootem client rawtest) do deltree /y %a\debug %a\release
