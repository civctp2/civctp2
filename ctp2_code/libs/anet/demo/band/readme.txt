
Using band - a Rough Guide
--------------------------
This is a possibly out of date excerpt from band.htm, which should
be referred to for details or if the below doesn't work.

Run dbuild2.bat to compile bandd.exe.
Run getdll.bat to get the needed anet exe's and dll's from
    elsewhere in the anet SDK.
Copy anetd.inf to anet.inf, so the debugging versions of all
    dll's and exe's are run.
Edit band.in or copy one of the example .in files to band.in.
Modify pushband.bat to push to all the computers you will be
    using in the test.  It should call 'pushband1.bat path' to copy
    the needed files to a working directory on each computer.  Run 
    the batch file.
Run hostd.bat on one computer and joind.bat on any others.
    This will run a test session using the parameters in the
    band.in file on the host and using the winets2d.dll driver.  
    An output.1 file will be created containing a log of packets sent
    and received.  Old output files will be moved to output.2, 3, etc.
    If there is an appropriate dp.ini file in the directory, a dp.log
    file containing debugging output will be also be created.
Modify getlog.bat to collect the output.1 files to one location.
    It should call 'getlog1.bat path tag' to get the dp.log and
    output.1 files from each to to the local filenames
    dp&lt;tag&gt;.log and out&lt;tag&gt;.1.  Run the batch file.
Run 'perl annotate.pl output.1 > out.log' to process the output
    logs.  This will add useful comments to the packet log, including
    bandwidth usage, latency, and some packet content info.
'perl annotate.pl output1.1 output2.1 > out12.log' for output
    logs from two computers in the same session will produce some
    additional packet loss annotation to the output1.1 log file.
Read or grep the annotated logs to see what happened.
