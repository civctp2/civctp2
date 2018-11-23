rem Regression test - make sure /wait works for at least a few commands.
rem Requires some other machine be hosting an ipx session.
rem $Log: test3.bat $
rem Revision 1.1  1997/02/26 00:54:40  dkegel
rem Initial revision

gtest -n=...\dos\dll\ipx.dll < test3.in
del test3.log
ren dp.log test3.log
