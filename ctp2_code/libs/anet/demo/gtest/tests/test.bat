rem Run test2 with each version of gtest.
set DLL=...\win\dll\wipx.dll
copy ...\win\dll\anetdll.dll
if %DLL%#==# goto fail
if not exist anetdll.dll goto fail
call test2 out\gtestd %DLL%
if errorlevel 1 goto fail
call test2 out\gtestdd %DLL%
if errorlevel 1 goto fail
echo Test passed.
goto end
:fail
echo Test failed.
:end
