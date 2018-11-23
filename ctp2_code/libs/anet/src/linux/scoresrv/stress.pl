#!/usr/bin/perl

use POSIX "sys_wait_h";

$JDKPATH="/usr/local/jdk";
system("export LD_LIBRARY_PATH=.");

select STDERR; $| = 1;	# line buffer so any error printing appears promptly
select STDOUT; $| = 1;	# line buffer in case we are piping to a log

if (! -x "antpclientd" || ! -x "wmqmerged") {
	die "required executables: antpclientd, wmqmerged\n";
}
if (! -x "$JDKPATH/bin/java_g") {
	die "required: $JDKPATH/bin/java_g";
}
if (! -r "libjscore2_g.so" || ! -r "score.jar" || ! -r "mysql.jar"
||  ! -r "servjar/score_65.jar" || ! -r "antpserv.lst") {
	die "
required files:
libjscore2_g.so, score.jar, mysql.jar, servjar/score_65.jar, antpserv.lst
";
}
foreach $dir ("etc1", "etc2", "etc3", "merged") {
	if (! -d $dir) {
		if (-e $dir) {
			die "$dir exists but is not a directory\n";
		}
		mkdir($dir, 0755) || die "can't mkdir $dir!";
	}
}
if (-e "antpclient.log" || -e "wmqmerge.log" || -e "jscore.log") {
	die "please make certain no copies of antpclient, wmqmerge, or jscore
are running in this directory and delete antpclient.log, wmqmerge.log, and
jscore.log before calling this script\n";
}
if (! -e "etc1/wmqmerge.dat" || ! -e "etc2/wmqmerge.dat"
||  ! -e "etc3/wmqmerge.dat" || ! -e "merged/wmqmerge.dat"
||  ! -e "merged/jscore.dat") {
	print STDERR "\n\
Warning: missing some startup files in wmq directories for wmqmerge or jscore.
If the wmq's on the servers have previous score reports for user9 (uid 66008),
this test will fail.  Delete etc*/*.dat and merged/* and run a 6 minute test
(which will fail) first to fix this problem.\n";
	sleep 2;
}
 
system('mysql alink -uscore -pr3f3r33 -e "delete from score_65 where uid=66008"');
system('mysql alink -uscore -pr3f3r33 -e "insert into score_65 values (66008, 0, 0, 0)"');

# these need to point to the servers that correspond to the antpserv's in
# antpserv.lst 
@server = ( 
	"206.17.228.147:11301",
	"206.17.228.147:11302",
	"206.17.228.147:11303"
);

$duration = 60;  # 1 hour test by default
if ($#ARGV >= 0) {
	$duration = $ARGV[0];
	if ($duration < 6) {
		die "test duration must be at least 6 min\n";
	}
}
$now = time();
$start = $now;
$end = $now + 60 * $duration;
$next_scorsend = $now;
$s = 0;
$n_sent = 0;
$next_restart_antp = $now;
$next_restart_wmqm = $now;
$next_restart_java = $now;
$next_reap = $now + 20;
$pid_antp = 0;
$pid_wmqm = 0;
$pid_java = 0;
@zombies = ();

while ($now < $end) {
	$now = time();

	if ($now >= $next_reap) {
		print "$n_sent scores sent, ", int(($end - $now)/60), "/",
			int(($end - $start)/60), " minutes remain in test\n";
		print "slaying ", 0+@zombies, " zombies at t:$now\n";
		@temp = @zombies;
		@zombies = ();
		foreach $pid (@temp) {
			if (($slain = waitpid($pid, 'WNOHANG')) > 0) {
				# print "slew zombie $slain\n";
			} else {
				push @zombies, $pid;
			}
		}
		$next_reap = $now + 20;
	}
	if ($now >= $next_restart_antp) {
		print "restarting antpclient ($pid_antp) at t:$now\n";
		if ($pid_antp > 0) {
			if (kill(9, $pid_antp) != 1) {
				die "antpclient died before restart!\n";
			}
			push @zombies, $pid_antp;
			$pid_antp = 0;
		}
		if (($pid_antp = fork()) > 0) {
			# parent process
			$next_restart_antp = $now + 15;
		} elsif (defined($pid_antp) && ($pid_antp == 0)) {
			# child process
			close STDIN;
			close STDOUT;
			close STDERR;
			open STDOUT, '>>antpclient.log'
				|| die "can't redirect to antpclient.log\n";
			open STDERR, ">&STDOUT" || die "can't send stderr to stdout\n";
			select STDERR; $| = 1;	# force line buffering	
			select STDOUT; $| = 1;	# force line buffering
			print STDOUT "********** Restart at t:$now **********\n";
			exec('./antpclientd', 'antpserv.lst');
			exit(0);
		} elsif ($! =~ /No more process/) {
			# parent error:EAGAIN, retry in 5 secs 
			print "fork returns EAGAIN on antpclient, retrying\n";
			$pid_antp = 0;
			$next_restart_antp = $now + 5;
		} else {
			# parent bad error
			die "fork error on antpclient:$!\n";
		}
	}	
	if ($now >= $next_restart_wmqm) {
		print "restarting wmqmerge ($pid_wmqm) at t:$now\n";
		if ($pid_wmqm > 0) {
			if (kill(9, $pid_wmqm) != 1) {
				die "wmqmerge died before restart!\n";
			}
			push @zombies, $pid_wmqm;
			$pid_wmqm = 0;
		}
		if (($pid_wmqm = fork()) > 0) {
			# parent process
			$next_restart_wmqm = $now + 16;
		} elsif (defined($pid_wmqm) && ($pid_wmqm == 0)) {
			# child process
			close STDIN;
			close STDOUT;
			close STDERR;
			open STDOUT, '>>wmqmerge.log'
				|| die "can't redirect to wmqmerge.log\n";
			open STDERR, ">&STDOUT" || die "can't send stderr to stdout\n";
			select STDERR; $| = 1;	# force line buffering	
			select STDOUT; $| = 1;	# force line buffering
			print STDOUT "********** Restart at t:$now **********\n";
			exec('./wmqmerged', 'merged', 'etc1', 'etc2', 'etc3');
			exit(0);
		} elsif ($! =~ /No more process/) {
			# parent error:EAGAIN, retry in 5 secs 
			print "fork returns EAGAIN on wmqmerge, retrying\n";
			$pid_wmqm = 0;
			$next_restart_wmqm = $now + 5;
		} else {
			# parent bad error
			die "fork error on wmqmerge:$!\n";
		}
	}	
	if ($now >= $next_restart_java) {
		print "restarting jscore ($pid_java) at t:$now\n";
		if ($pid_java > 0) {
			if (kill(9, $pid_java) != 1) {
				die "jscore died before restart!\n";
			}
			push @zombies, $pid_java;
			$pid_java = 0;
		}
		if (($pid_java = fork()) > 0) {
			# parent process
			$next_restart_java = $now + 17;
		} elsif (defined($pid_java) && ($pid_java == 0)) {
			# child process
			close STDIN;
			close STDOUT;
			close STDERR;
			open STDOUT, '>>jscore.log'
				|| die "can't redirect to jscore.log\n";
			open STDERR, ">&STDOUT" || die "can't send stderr to stdout\n";
			select STDERR; $| = 1;	# force line buffering	
			select STDOUT; $| = 1;	# force line buffering
			print STDOUT "********** Restart at t:$now **********\n";
			exec("$JDKPATH/bin/java_g",
				 '-classpath', "$JDKPATH/lib/classes.zip:score.jar:mysql.jar",
				 'Activision.score_main', 'merged',
				 '206.17.228.147/alink?user=score;password=r3f3r33');
			exit(0);
		} elsif ($! =~ /No more process/) {
			# parent error:EAGAIN, retry in 5 secs 
			print "fork returns EAGAIN on jscore, retrying\n";
			$pid_java = 0;
			$next_restart_java = $now + 5;
		} else {
			# parent bad error
			die "fork error on jscore:$!\n";
		}
	}
	if ($now < $end - 400) {
		# stop sending scores long enough before end (~6 min) to make sure
		# all sent scores go through.
		if ($now >= $next_scorsend) {
			print "sending score to $server[$s] at t:$now\n";
			$err = system("./scorsend -n=foo -j -g=$server[$s] -z=1 -u=user9 pass9 > /dev/null");
			$now = time();  # much time may have elapsed
			$s++;
			if ($s > $#server) {
				$s = 0;
			}
			$n_sent++;
			if (!($n_sent % 20)) {
				# wait long enough for the wmq to skip a file
				$next_scorsend = $now + 200;
			} elsif (!($n_sent % 10)) {
				# wait long enough to end up in the next wmq file
				$next_scorsent = $now + 100;
			} else {
				$next_scorsend = $now + 1;
			}
		}
	}
}

# kill everything and check the database for correct score
print "checking results...\n";
if ($pid_antp && kill(9, $pid_antp) != 1) {
	print "antpclient died?\n";
}
if ($pid_wmqm && kill(9, $pid_wmqm) != 1) {
	print "wmqmerge died?\n";
}
if ($pid_java && kill(9, $pid_java) != 1) {
	print "jscore died?\n";
}

if ($pid = open(MYSQL, "-|")) {
	$mysql_out = <MYSQL>;
	if ($mysql_out !~ /sum_score/) {
		print "wierd mysql output:$mysql_out ...\n";
	}
	$mysql_out = <MYSQL>;
	$sum_score = $mysql_out + 0;
	if ($sum_score != $n_sent) {
		print "sum_score:$sum_score != expected:$n_sent\n";
		exit(1);
	} else {
		print "Test passed!  sum_score = $sum_score as expected\n";
	}
	close(MYSQL);
} elsif (defined($pid)) {
	exec('mysql alink -uscore -pr3f3r33 -B -e "select sum_score from score_65 where uid=66008"');
	exit(0);
} else {
	die "can't fork mysql select:$!\n";
}
exit(0);
