while (<>) 
{
	if (/Setting server to (\S+)\./) {
		$serv = $1;	
		if ($min{$serv} eq "") {
			$min{$serv} = 999;
		}
	} 
	elsif(/Loader0 in.* = +(\S+)\./) {
		$t = $1;
		$fail{$serv}++ if ($t =~ /\*/);
		next if ($t == 0);
		$sum{$serv} += $t;
		$min{$serv} = $t if ($t < $min{$serv});
		$max{$serv} = $t if ($t > $max{$serv});
		$n{$serv}++;
	}
}

foreach $serv (keys(%n)) {
	$avg = sprintf("%4.1f", $sum{$serv} / $n{$serv});  
	$fail = sprintf("%3d", 100 * $fail{$serv} / ($n{$serv} + $fail{$serv}));  
	print $serv." min $min{$serv},  avg $avg, max $max{$serv}, n $n{$serv}, fail ". (0 + $fail{$serv})." = $fail%\n";
}
