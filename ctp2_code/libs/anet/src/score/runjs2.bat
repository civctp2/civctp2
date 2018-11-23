SET JDKPATH=d:\jdk
copy ..\3rdparty\mysql.jar .
%JDKPATH%\bin\java -classpath %JDKPATH%\lib\classes.zip;score.jar;mysql.jar Activision.score_init %1 %2
