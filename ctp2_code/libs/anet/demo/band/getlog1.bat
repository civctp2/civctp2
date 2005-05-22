if "%1"=="" goto err
if "%2"=="" goto err

copy %1\dp.log dp%2.log
copy %1\output.1 out%2.1
perl annotate.pl out%2.1 > out%2.log
goto end

:err
@echo Usage: getlog1 souce_path id
@echo     gets dp.log and output.1 from source_path, renames them to
@echo     dp[id].log and out[id].1, then annotates out[id].1 > out[id].log

:end
