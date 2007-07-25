#!/bin/sh
set -e
set -x

# Debugging + logging
#FLAGS='-DDEBUG -g -D_DEBUG'
# Logging
#FLAGS='-DDPRNT -O3'
# Profiling
#FLAGS='-DNDEBUG -O3 -pg'
# Normal
#FLAGS='-DNDEBUG -O3'
FLAGS='-g'

COPT="${FLAGS} -DUNIX -Dcdecl= -D__NO_ANONYMOUS_UNIONS__ -Dstrnicmp=strncasecmp -Dstricmp=strcasecmp -Ddp_ANET2"
MYJDKPATH=/usr/local/jdk
MYCLASSPATH=.:${MYJDKPATH}/lib/classes.zip

cd Activision
javac -classpath ${MYCLASSPATH} myLoader.java
javac -classpath ${MYCLASSPATH} ScoreServletHandler.java
javac -classpath ${MYCLASSPATH} score_init.java
javac -classpath ${MYCLASSPATH} ScoreServlet.java
javac -classpath ${MYCLASSPATH} ScoreReport.java
javah -classpath ${MYCLASSPATH} -jni Activision.score_init

gcc ${COPT} "-DPACK=__attribute__ ((packed))" -DTEST_MAIN -I${MYJDKPATH}/include -I${MYJDKPATH}/include/genunix -I../../h -I. -c jscore2.c 
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -DTEST_MAIN -I${MYJDKPATH}/include -I${MYJDKPATH}/include/genunix -I../../h -I. -c sq.c 
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -DTEST_MAIN -I${MYJDKPATH}/include -I${MYJDKPATH}/include/genunix -I../../h -I. -c wmq.c 
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -DTEST_MAIN -I${MYJDKPATH}/include -I${MYJDKPATH}/include/genunix -I../../h -I. -c wmqfile.c 
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -DTEST_MAIN -I${MYJDKPATH}/include -I${MYJDKPATH}/include/genunix -I../../h -I. -c scorerep.c 
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -DTEST_MAIN -I${MYJDKPATH}/include -I${MYJDKPATH}/include/genunix -I../../h -c ../dp/assoctab.c
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -DTEST_MAIN -I${MYJDKPATH}/include -I${MYJDKPATH}/include/genunix -I../../h -c ../dp/dynatab.c
gcc ${COPT} "-DPACK=__attribute__ ((packed))" -DTEST_MAIN -shared jscore2.o sq.o wmq.o wmqfile.o scorerep.o assoctab.o dynatab.o -L${MYJDKPATH}/lib/i586/green_threads -ljava_g -o libjscore2_g.so

cd ..
jar -cvf0 score.jar Activision/*.class

MYCLASSPATH=${MYJDKPATH}:score.jar
javac -classpath ${MYCLASSPATH} score_65.java
javac -classpath ${MYCLASSPATH} session_65.java
javac -classpath ${MYCLASSPATH} scoreset_65.java
javac -classpath ${MYCLASSPATH} scorerep_65.java
jar -cvf0 score_65.jar *_65.class *_65$*.class

rm -rf servjar
mkdir servjar
mv score_*.jar servjar
