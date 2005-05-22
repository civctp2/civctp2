if "%1"=="" goto err

copy *.exe %1
copy anet*.dll %1
copy dp.ini %1
copy msvcrtd.dll %1
copy join*.bat %1
copy host*.bat %1
copy *.in %1
copy anet*.inf %1
mkdir %1\anet
copy anet\*.dll %1\anet
copy anet\*.ini %1\anet
goto end

:err
echo Usage: pushband1.bat path
echo      copies all the files necessary to run the demo to path

:end
