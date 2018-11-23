# Analyze a dp.log file or set of dp.log files.
# Assumes that master's log file is psp0.log
# and any clients are psp1.log, psp2.log, etc.
# Relies on time in dp.log's being from same timebase (e.g. millisec since
# midnight).
# Written originally for src/test/psp to pinpoint what was causing
# delays in joining a session.
# (The bottom line was- the master was spending so much writing the log file
#  it couldn't service all the packets it was getting before its timeslice
#  ran out, so they stacked up, and lag got to be 20 seconds with 16 hosts.)
#
# Known bug: assumes no game server is used, i.e. hMaster is 2.  Easily fixed.
#
# $Log: analog.pl $
# Revision 1.4  1997/09/18 18:53:44  anitalee
# ordered the output messages.
# Revision 1.3  1997/09/18 18:09:10  anitalee
# added sensing of player rx/tx
# Revision 1.2  1997/09/17 17:28:24  lbennett
# Expects server.log, too
# Seems to work again
# Revision 1.1  1997/09/17 04:03:33  lbennett
# Initial revision
# Revision 1.1  1997/08/30 21:59:26  dkegel
# Initial revision

# Return the given number in little endian hex format.
sub lehex
{
	local ($out);
	$out = sprintf("%x", $_[0]);
	return substr($out, 2, 2).substr($out,0,2);
}

sub analyse
{
	local($i) = $_[0];
	local($clockscale) = $_[1];
	local (%h2adr, $myadr, $h, $adr, $lastPktnum, $joinPktnumt, $qpkt);

	while (<IN>) {
		# Events more or less in order of occurance in normal lifetime
		if (/ t:(\d+)/) {
			$atime = $1;
			$time = ($atime - $start) * $clockscale;
		}
		if (/dpio_create: myAdr = (\S+)/) {
			$myadr = $1;
			$myadr =~ s/://g;
			print "$i $time: address is $myadr\n";
			$i2adr{$i} = $myadr;
			$adr2i{$myadr} = $i;
			$h2adr{"fffffffe"} = $myadr;
			$h2adr{"ffffffff"} = "bcast";
			if ($i != 0) {
				print "debug: have createplayer pktnum $pkt{$myadr}\n";
			}
		}
		if (/dpio_openHdlRaw: adr (\S+) .* h:(\S+)/) {
			$adr = $1;
			$h = $2;
			$adr =~ s/://g;
			print "$i $time: h $h maps to adr $adr\n";
			$h2adr{$h} = $adr;
		}
		if (/dpio_put_unreliable: t:(\d+) tag dY ln \d+ sent to h:([\da-f]+)/) {
			$h = $2;
			$adr = $h2adr{$h};
			if ($i == 0) {
				print "$i $time: Sent syn to slave $h ($adr).\n";
				$m = $masterSentSynToSlave{$adr};
				$m .= "," if ($m ne "");
				$m .= $time;
				$masterSentSynToSlave{$adr} = $m;
			} elsif ($h == 2) {					# hMaster better be 2
				print "$i $time: $h ($adr) sent syn to master.\n";
				$m = $slaveSentSynToMaster{$myadr};
				$m .= "," if ($m ne "");
				$m .= $time;
				$slaveSentSynToMaster{$myadr} = $m;
			}
		}
		if (/dpio_get: sent ack for syn: dest h:([\da-f]+) .* pktnum (\d+), t:(\d+)/) {
			$h = $1;
			$adr = $h2adr{$h};
			$pktnum = $2;
			$hpktnum = &lehex($pktnum);
			if ($i == 0) {
				print "$i $time: Sent syn ack $pktnum ($hpktnum) to slave $h ($adr).\n";
				$m = $masterSentSynAckToSlave{$adr};
				$m .= "," if ($m ne "");
				$m .= $time;
				$masterSentSynAckToSlave{$adr} = $m;
			} elsif ($h == 2) {					# hMaster better be 2
				print "$i $time: ($myadr) sent syn ack $pktnum ($hpktnum) to master.\n";
				$m = $slaveSentSynAckToMaster{$myadr};
				$m .= "," if ($m ne "");
				$m .= $time;
				$slaveSentSynAckToMaster{$myadr} = $m;
			}
		}

		if (/Connection established with h:([\da-f]+) state:7 t:(\d+)/) {
			$h = $1;
			$adr = $h2adr{$h};
			if ($i == 0) {
				print "$i $time: Established connection with slave $h ($adr).\n";
				$gotSlaveConn{$adr} = $time;
			} elsif ($h == 2) {					# hMaster better be 2
				print "$i $time: Established connection with master.\n";
				$gotMasterConn{$myadr} = $time;
			}
		}
		if (/dpio_q_packet: queueing; tag dJ .* pktnum (\d+)/) {
			$lastPktnum = $1;
		}
		if (/dpio_q_packet: .* sent tag dJ .* pktnum (\d+)/) {
			$lastPktnum = $1;
		}
		if (/dpOpen: join.*t:(\d+)/) {
			$joinPktnum = $lastPktnum;
			$hpktnum = &lehex($joinPktnum);
			print "$i $time $atime: sent join request pktnum $joinPktnum ($hpktnum)\n";
			$sentJoin{$i} = $time;
		}
		if (/dpio_update: t:(\d+) sent pktnum (\d+) tag dJ/) {
			if ($2 != $joinPktnum) {
				print "Hey: got pktnum $2, expected $joinPktnum\n";
				die;
			}
			print "$i $time: Resent join\n";
			$sentJoin{$i} .= ",$time";
		}
		if (/dpHandleJoinSession: src h:([\da-f]+),.*t:(\d+)/) {
			$h = $1;
			$adr = $h2adr{$h};
			print "$i $time: got join request from $h ($adr)\n";
			$gotJoin{$adr} = $time;
		}
		if (/dpio_get: t:(\d+) got data packet: pktnum (\d+) from src h:([\da-f]+) tag dJ; acking/) {
			$h = $3;
			$adr = $h2adr{$h};
			$pktnum = $2;
			$hpktnum = &lehex($pktnum);
			if ($ackJoin{$adr} ne "") {
				$ackJoin{$adr} .= ",".$time;
			} else {
				$ackJoin{$adr} .= $time;
			}
			print "$i $time: acking join pktnum $pktnum ($hpktnum) from $h ($adr)\n";
		}
		if (/dptab_set\(table.4\..*t:(\d+)/) {
			print "$i $time $atime: requested player created\n";
			$reqPlayerCreateTx{$i} = $time;
		}
		if ($i == 0) {
			if (/dptab_set\(table.3.*key ([\.\d]+),.*h:([\da-f]+)\):/) {
				$h = $2;
				$adr = $h2adr{$h};
				$key{$adr} = $1;
				$reqPlayerCreateRx{$adr} = $time;
				print "$i $time: got create player request from $h ($adr)\n";
			}
			if (/dpio_q_packet:.*pktnum (\d+)/) {
				$qpkt = $1;
			}
			if (/flush: sent table 3.* subkey ([\.\d]+) .*; h:([\da-f]+)/) {
				$h = $2;
				$adr = $h2adr{$h};
				if ($1 eq $key{$adr}) {
					$pkt{$adr} = $qpkt;
					$respPlayerCreateTx{$adr} = $time;
					print "$i $time: sent create player response to $h ($adr)\n";
				}
			}
			if (/dpio_update:.*sent pktnum (\d+) .* h:([\da-f]+)/) {
				$h = $2;
				$adr = $h2adr{$h};
				if ($1 eq $pkt{$adr}) {
					$respPlayerCreateTx{$adr} .= ",$time";
					print "$i $time: resent create player response to $h ($adr)\n";
				}
			}
			if (/dpio_get:.*got ack: src h:([\da-f]+), pktnum (\d+)/) {
				$h = $1;
				$adr = $h2adr{$h};
				if ($2 eq $pkt{$adr}) {
					if ($ackrespPlayerCreate{$adr} eq "") {
						$ackrespPlayerCreateTx{$adr} = "$time";
					} else {
						$ackrespPlayerCreateTx{$adr} .= ",$time";
					}
					print "$i $time: got ack for create player response to $h ($adr)\n";
				}
			}
		} else {
			if (/dpio_get:.*pktnum (\d+).*; acking/) {
				if ($1 eq $pkt{$myadr}) {
					if ($gotrespPlayerCreate{$i} eq "") {
						$gotrespPlayerCreate{$i} = "$time";
					} else {
						$gotrespPlayerCreate{$i} .= ",$time";
					}
					print "$i $time: got create player response\n";
				}
			}
		}
		if (/dp_player_notify: local player, calling callback./) {
			print "$i $time $atime: player created\n";
			$respPlayerCreateRx{$i} = $time;
		}
		if (/^rx: +(\d+) +([\da-f]+) +(.*)/) {
			$h = $2;
			$adr = $h2adr{$h};
			$data = $3;
			print "$i $time: rx $time $adr $data\n";
		}
		if (/^tx: +(\d+) +([\da-f]+) +(.*)/) {
			$h = $2;
			$adr = $h2adr{$h};
			$adr = "??" if ($adr eq "");
			$data = $3;
			print "$i $time: tx $time $adr $data\n";
		}
	}
}


# Figure out $start 
$start = 0;
open(IN, "server.log") || last;
while (<IN>) {
	if (/dpHandleJoinSession: src h:[^f].*t:(\d+)/) {
		$start = $1;
		last;
	}
}
close(IN);

if (open(IN, "server.log")) {
	&analyse(0, 10);
	close(IN);
} else {
	warn "Can't open server.log";
}

# Figure out $start 
$start = 0;
for ($i=0; ; $i++) {
	open(IN, "load$i.log") || last;
	while (<IN>) {
		if (/dpOpen: join.*t:(\d+)/) {
			if (($start == 0) || ($1 < $start)) {
				$start = $1;
			}
			last;
		}
	}
	close(IN);
}
for ($i=0; ; $i++) {
	open(IN, "load$i.log") || last;
	&analyse($i+1, 1);
	close(IN);
}
$nhosts = $i + 1;

sub ptimes
{
	foreach (split(",",$_[0])) {
		print $_. ",";
	}
}

print "Run started at $start\n";
foreach $i (0..$nhosts-1) {
	$adr = $i2adr{$i};
	print "$i:\n";

	print "Slave tx syn  ";	&ptimes($slaveSentSynToMaster{$adr});
	print "; Mastr ack syn ";	&ptimes($masterSentSynAckToSlave{$adr});

	print "\nMastr tx syn  ";	&ptimes($masterSentSynToSlave{$adr});
	print "; Slave ack syn ";	&ptimes($slaveSentSynAckToMaster{$adr});

	print "\nSlave rx conn ". $gotMasterConn{$adr} . ", ";
	print "Master rx conn ". ($gotSlaveConn{$adr}).", ";
	print "\nJoin tx ";	        &ptimes($sentJoin{$i});
	print "; ";
	print "ack ";	&ptimes($ackJoin{$adr});
	print "rx ".     $gotJoin{$adr}. ", ";
	print "\nSlave tx Plr (tab)". $reqPlayerCreateTx{$i}. ", ";
	print "\Master rx Plr (tab)". $reqPlayerCreateRx{$adr}. ", ";
	print "\nMaster tx Plr (io)". $respPlayerCreateTx{$adr}. ", ";
	print "\Slave rx Plr (io)". $gotrespPlayerCreate{$i}. ", ";
	print "\nMastr rx ack Plr (io)". $ackrespPlayerCreateTx{$adr}. ", ";
	print "Slave rx Plr (tab)". $respPlayerCreateRx{$i}. ", ";
	print "\n";
}


