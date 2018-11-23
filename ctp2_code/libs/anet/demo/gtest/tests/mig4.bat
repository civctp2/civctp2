rem Test host migration in a four-player game.
rem For some reason, fails the first time you run it, so run it twice.
rem Watch the output windows to make sure that players foo1, foo2, and foo3
rem all make it in at the end of the test.
deltree /y user?
for %%i in (0 1 2 3) do start mig.bat %%i mig
