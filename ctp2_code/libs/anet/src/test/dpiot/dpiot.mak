# unit test for dpio.

# kludgy way to make sure correct defines used for loopback and internet drivers
dpiot.exe: dpiot.c
	cl -W3 -Zi /MDd -DDPRNT -Ddp_ANET2 dpiot.c ../../../demo/utils/unistd.c -I../../../h ../../../win/lib/dp2d.lib

dpiot.loop: dpiot.c
	cl -W3 -Zi /MDd -DDPRNT -Ddp_ANET2 -DLOOP dpiot.c ../../../demo/utils/unistd.c -I../../../h ../../../win/lib/dp2d.lib

dpiot.internet: dpiot.c
	cl -W3 -Zi /MDd -DDPRNT -Ddp_ANET2 -DINET dpiot.c ../../../demo/utils/unistd.c -I../../../h ../../../win/lib/dp2d.lib

# IPX address to connect to if testing IPX
IPXADR=0.0.16.0,0.aa.0.b7.13.e

clean:
	-for %%a in (*.obj) do del %%a
	-for %%a in (*.lib) do del %%a
	-for %%a in (*.exe) do del %%a
	-for %%a in (*.exp) do del %%a
	-for %%a in (*.pdb) do del %%a
	-for %%a in (*.ilk) do del %%a
	-for %%a in (*.log) do del %%a

#Loopback Tests
test: dpiot.loop ../../../win/dll/wloopd.dll
	dpiot -d=../../../win/dll/wloopd.dll -h=10 -l=2 -nofreeze

test2: dpiot.loop ../../../win/dll/wloopd.dll
	dpiot -d=../../../win/dll/wloopd.dll -h=2 -l=5 -nofreeze

test3: dpiot.loop ../../../win/dll/wloopd.dll
	dpiot -d=../../../win/dll/wloopd.dll -h=3 -l=5 -nofreeze

test5: dpiot.loop ../../../win/dll/wloopd.dll
	dpiot -d=../../../win/dll/wloopd.dll -h=5 -l=4 -nofreeze

test2p: dpiot.loop ../../../win/dll/wloopd.dll
	dpiot -d=../../../win/dll/wloopd.dll -h=2 -l=1 -p=200 -s=200 -nofreeze

test3p: dpiot.loop ../../../win/dll/wloopd.dll
	dpiot -d=../../../win/dll/wloopd.dll -h=3 -l=1 -p=100 -s=200 -nofreeze

#Internet Tests - change INET_ADDR to be your own address in dpiot.c
testi2: dpiot.internet ../../../win/dll/winets2d.dll
	dpiot -d=../../../win/dll/winets2d.dll -h=2 -l=5 -nofreeze

testi3: dpiot.internet ../../../win/dll/winets2d.dll
	dpiot -d=../../../win/dll/winets2d.dll -h=3 -l=5 -nofreeze

testi3f: dpiot.internet ../../../win/dll/winets2d.dll
	dpiot -d=../../../win/dll/winets2d.dll -h=3 -l=5

testi2p: dpiot.internet ../../../win/dll/winets2d.dll
	dpiot -d=../../../win/dll/winets2d.dll -h=2 -l=1 -p=200 -s=200 -nofreeze

testi3p: dpiot.internet ../../../win/dll/winets2d.dll
	dpiot -d=../../../win/dll/winets2d.dll -h=3 -l=1 -p=100 -s=200 -nofreeze

#Modem Tests
testm2: dpiot.exe ../../../win/dll/wmodem2d.dll
	dpiot -d=../../../win/dll/wmodem2d.dll -h=2 -l=5 -n=1 -a=SERIAL:1 -y=1 -nofreeze

testm2p: dpiot.exe ../../../win/dll/wmodem2d.dll
	dpiot -d=../../../win/dll/wmodem2d.dll -h=2 -l=1 -p=200 -s=200 -n=1 -a=SERIAL:1 -y=1 -nofreeze

#Serial Tests
tests2: dpiot.exe ../../../win/dll/wserial2d.dll
	dpiot -d=../../../win/dll/wserial2d.dll -h=2 -l=5 -n=1 -a=SERIAL:1 -y=1 -nofreeze

tests2p: dpiot.exe ../../../win/dll/wserial2d.dll
	dpiot -d=../../../win/dll/wserial2d.dll -h=2 -l=1 -p=200 -s=200 -n=1 -a=SERIAL:1 -y=1 -nofreeze

#IPX Tests
testx2: dpiot.exe ../../../win/dll/wipx2d.dll
	dpiot -d=../../../win/dll/wipx2d.dll -h=2 -l=1 -n=1 -a=$(IPXADR) -nofreeze

testx2p: dpiot.exe ../../../win/dll/wipx2d.dll
	dpiot -d=../../../win/dll/wipx2d.dll -h=2 -l=1 -p=200 -s=200 -n=1 -a=$(IPXADR) -nofreeze

