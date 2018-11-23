echo Getting output.1 and dp.log files from all computers...

rem If you are running this in a directory you've created for processing logs,
rem you might want to uncomment these lines to get rid of old logs first
rem del *.log
rem del *.1

call getlog1.bat \\dkegel1\band 1
call getlog1.bat \\dkegel3\band 3
call getlog1.bat \\dkegel06\band 6
call getlog1.bat \\dkegel9\band 9

