del *.log 
del *.out

rem Master server, 1.0.0.0
start dp2t2 m1.log > m1.out

rem Slave server, 2.0.0.0
start dp2t2 s1.log  1.0.0.0 > s1.out

rem Client, 3.0.0.0, connected to slave server
start dp2t1 c11.log 2.0.0.0 > c11.out

rem 2nd Slave server, 4.0.0.0
start dp2t2 s2.log  1.0.0.0 > s2.out

rem Client, 5.0.0.0, connected to 2nd slave server
start dp2t1 c21.log 4.0.0.0 > c21.out

rem Clients connected to 1st slave server
start dp2t1 c12.log 2.0.0.0 > c12.out
start dp2t1 c13.log 2.0.0.0 > c13.out
start dp2t1 c14.log 2.0.0.0 > c14.out
start dp2t1 c15.log 2.0.0.0 > c15.out

rem Clients connected to 2nd slave server
start dp2t1 c22.log 4.0.0.0 > c22.out
start dp2t1 c23.log 4.0.0.0 > c23.out
start dp2t1 c24.log 4.0.0.0 > c24.out
start dp2t1 c25.log 4.0.0.0 > c25.out

