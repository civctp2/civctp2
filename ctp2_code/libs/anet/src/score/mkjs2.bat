cd Activision

set MYCLASSPATH=.;..;d:\jdk\lib\classes.zip
javac -classpath %MYCLASSPATH% myLoader.java
if errorlevel 1 goto bad
javac -classpath %MYCLASSPATH% ScoreServletHandler.java
if errorlevel 1 goto bad
javac -classpath %MYCLASSPATH% score_init.java
if errorlevel 1 goto bad
javac -classpath %MYCLASSPATH% ScoreServlet.java
if errorlevel 1 goto bad
javac -classpath %MYCLASSPATH% ScoreReport.java
if errorlevel 1 goto bad
javah -classpath %MYCLASSPATH% -jni Activision.score_init
if errorlevel 1 goto bad

set OLDINC=%INCLUDE%
set INCLUDE=%INCLUDE%;d:\jdk\include;d:\jdk\include\win32;..\..\..\h;..\..\dp;..\..\..\demo\utils;..
cl /D_DEBUG /D_WIN32 /DWIN32 /W3 /Zi jscore2.c ..\scorerep.c ..\sq.c ..\wmq.c ..\wmqfile.c ..\..\dp\assoctab.c ..\..\dp\dynatab.c ..\..\..\demo\utils\win32dirent.c ..\..\..\demo\utils\win32direntry.c ..\..\..\win\lib\anet2d.lib d:\jdk\lib\javai.lib /LD /Fejscore2.dll
set INCLUDE=%OLDINC%
if errorlevel 1 goto bad

cd ..
jar -cvf0 score.jar Activision/*.class
if errorlevel 1 goto bad

set MYCLASSPATH=%MYCLASSPATH%;score.jar
javac -classpath %MYCLASSPATH% score_65.java
if errorlevel 1 goto bad
javac -classpath %MYCLASSPATH% session_65.java
if errorlevel 1 goto bad
javac -classpath %MYCLASSPATH% scoreset_65.java
if errorlevel 1 goto bad
javac -classpath %MYCLASSPATH% scorerep_65.java
if errorlevel 1 goto bad
jar -cvf0 score_65.jar *_65.class *_65$*.class
if errorlevel 1 goto bad

deltree /y servjar
mkdir servjar
mv score_*.jar servjar
if errorlevel 1 goto bad

copy Activision\jscore2.dll .

echo Build successful
goto end

:bad
echo Build failed
:end

