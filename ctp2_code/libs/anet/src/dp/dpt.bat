del *.log 
del *.out

rem Master server, 1.0.0.0
start dp2t2 m1.log > m1.out

rem Slave server, 2.0.0.0
start dp2t2 s1.log 1.0.0.0 > s1.out

rem Client, 3.0.0.0, connected to slave server
start dp2t1 c1.log 2.0.0.0 > c1.out

rem 2nd Slave server, 4.0.0.0
start dp2t2 s2.log 1.0.0.0 > s2.out

rem Client, 5.0.0.0, connected to 2nd slave server
start dp2t1 c2.log 4.0.0.0 > c2.out
