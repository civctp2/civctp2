set MYCLASSPATH=.;d:\jdk\lib\classes.zip
javac -classpath %MYCLASSPATH% score_65.java
jar -cvf0 score_65.jar score_65*.class
