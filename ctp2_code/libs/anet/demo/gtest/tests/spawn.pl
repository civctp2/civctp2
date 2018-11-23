$x = 0;
while ($x < $ARGV[0]) {
    Win32::Spawn('c:\windows\command\start.exe', "start /m spawn $x", $pid);
	print "$pid\n";
	if (!$pid) {
		print Win32::FormatMessage(Win32::GetLastError);
	}
    sleep 5;
	$x++;
}
