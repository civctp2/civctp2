# unit test for dptab.

dptabt.exe: dptabt.c
	cl /DLOOPBACK_ADDRESS -Fm -W3 -Zi /MDd -DDPRNT dptabt.c ../../../demo/utils/unistd.c -I../../../h ../../../win/lib/dp2d.lib kernel32.lib user32.lib advapi32.lib version.lib

clean:
	-for %%a in (*.obj) do del %%a
	-for %%a in (*.lib) do del %%a
	-for %%a in (*.exe) do del %%a
	-for %%a in (*.exp) do del %%a
	-for %%a in (*.pdb) do del %%a
	-for %%a in (*.ilk) do del %%a
	-for %%a in (*.log) do del %%a

test: dptabt.exe ../../../win/dll/wloop2d.dll
	dptabt ../../../win/dll/wloop2d.dll 10 2

test2: dptabt.exe ../../../win/dll/wloop2d.dll
	dptabt ../../../win/dll/wloop2d.dll 2
