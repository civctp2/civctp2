# Simple perl script to annotate packet logs (usually named output.1) 
# written by debug versions of Activenet transport drivers (e.g. wheat2d.dll)
# Makes the logs more human-readable.
# Handles somewhat specialized packets used by demo/band.c and Battlezone.
# Also prints out a few statistics at end.
#
# Jason Scanlin / Dan Kegel  July 1998
# Copyright 1998 Activision

#$ECLOCKS_PER_SEC = 1000;  # for windows
$ECLOCKS_PER_SEC = 100;  # for linux

unless (@ARGV > 0) {
    print <<"END";
Usage: pkttrack.pl filename1 [filename2]
    compare packet log files dumped by wheat2d.dll to measure net stats
    for packets sent by filename1 to filename2 if filename2 is given
END
    ;
    exit(0);
}

# If 2nd file given, read it and note which packets it received.
# Used only for calculating packet loss.
if (@ARGV > 1) {
	print "reading rx position packets from $ARGV[1]...\n"; 
	open(DATFILE, "$ARGV[1]") or die "failed to open $ARGV[1]\n";
	%rx = ();
	while ($line = <DATFILE>) {
		# Parse line
		if ($line !~ /^(..) +(\d+) +([-\d]+) +(\d+) : (..) (.*)/) {
			next;
		}
		$tag = $1;
		$time = $2;
		$peer = $3;
		$length = $4;
		$pkt_type = $5;
		$body = $6;

		# Kludge: only note packet loss of packets we know from tag are 
		# unreliable broadcast
		if ($tag eq "rx" and $pkt_type =~ /^(_|B_)/) {
			$binpacket = pack("H*", $body);
			if (exists($rx{$binpacket})) {
				print "warning: duplicate packet from $peer : $line\n";
			} else {
				$rx{$binpacket}++;
			}
		}
	}
	close(DATFILE);
}

# Read main packet log.  Print annotated version to stdout.

$SENT_LIST_LENGTH = 200;
@sent_time = (0) x $SENT_LIST_LENGTH;
@sent_pktnum = (0) x $SENT_LIST_LENGTH;
@last_t = ();
$initime = 0;
print "reading packets from $ARGV[0]...\n";
open(DATFILE, "$ARGV[0]") or die "failed to open $ARGV[0]\n";
while ($line = <DATFILE>) {
    # Parse line
    if ($line !~ /^(..) +(\d+) +([-\d]+) +(\d+) : (..) (.*)/) {
		next;
    }
    $tag = $1;
    $time = $2;
    $peer = $3;
    $length = $4;
	$pkt_type = $5;
    $body = $6;

	$pktinfo = sprintf("%s $time h:%-2d", $tag, $peer);

	if ($initime == 0) {
		$initime = $time;
	}
    $timestr = sprintf("%6.2f", ((($time-$initime)/($ECLOCKS_PER_SEC/100.0))%100000)/100.0);
    # Note bandwidth usage
    if (int($time / $ECLOCKS_PER_SEC) != int($last_time / $ECLOCKS_PER_SEC)) {
		printf("rx %5d bytes, %3d pkts, tx %5d bytes, %3d pkts/sec\n",$bytes_received,$pkts_received,$bytes_sent,$pkts_sent);
		$bytes_received = 0;
		$pkts_received = 0;
		$bytes_sent = 0;
		$pkts_sent = 0;
    }
    if ($tag eq "rx") {
		$bytes_received += $length;
		$pkts_received++;
    } elsif ($tag eq "tx") {
		$bytes_sent += $length;
		$pkts_sent++;
    }
    $last_time = $time;

	&print_packet($timestr, $tag, $pkt_type, $body, $pktinfo, $length);
}
close(DATFILE);

print "Statistics:\n";

print "Piggyback ping:\n";
foreach (sort { $a - $b } (keys(%hist_ping))) {
	print " ".$_."0  ".$hist_ping{$_}."\n";
}

print "DT (total network dropout interval):\n";
foreach (sort { $a - $b } (keys(%hist_DT))) {
	print " $_ ".$hist_DT{$_}."\n";
}

exit;


sub print_packet()
{
	local ($timestr, $tag, $pkt_type, $body, $pktinfo, $length) = @_;

	$annotation = $timestr;
	
	$wrap_type = "";
	if ($pkt_type eq "dT") {
		if ($body =~ /^.. .. .. .. .. (.. ..) ?(.*)$/) {
			$wraptypehex = join "", (split /\s/, $1);  # bigendian
			$wrap_type = pack("H4", $wraptypehex);
			$wrap_body = $2;
			if ($wrap_type eq "d%") {
				$wrap_body =~ /^.. (..) (..) (.*)$/;
				$keylen = unpack("S", pack("H4", "${2}00"));
				$subkeylen = unpack("S", pack("H4", "${1}00"));
				$tablekeyhex = join "", ((split /\s/, $3)[0..($keylen-1)]);
				$wrap_type .= "$tablekeyhex";
				#$datahex = join "", ((split /\s/, $3)[$keylen..]);
				
			} elsif ($wrap_type eq "d!") {
				$wrap_body =~ /^.. .. .. .. .. .. .. .. .. (..) (..) .. ?(.*)$/;
				$keylen = unpack("S", pack("H4", "${1}00"));
				$subkeylen = unpack("S", pack("H4", "${2}00"));
				$tablekeyhex = join "", ((split /\s/, $3)[0..($keylen-1)]);
				$tablesubkeyhex = join "", ((split /\s/, $3)[$keylen..($keylen+$subkeylen-1)]);
				$wrap_type .= "$tablekeyhex.$tablesubkeyhex";
			} elsif ($wrap_type eq "d(") {
				$wrap_body =~ /^(..)/;
				$tcatype = pack("H2", $1);
				$wrap_type .= "$tcatype";
			}
		}
	}
	if ($pkt_type =~ /d[TUYF]/) {
		$body =~ /^.. .. (.. ..) (..)/;
		$seqnumhex = join "", (split /\s/, $1);  # bigendian
		$seqnum = unpack("S", pack("H*", $seqnumhex));
		$nack = unpack("S", pack("H*", "${2}00"));
	}
	
	if ($pkt_type =~ /d[TUYF]/) {
		$annotation .= " ";
		$annotation .= " " if $tag eq "rx" xor $pkt_type eq "dU";
		$annotation .= "\#${seqnum}";
	}
	if ($pkt_type eq "dT") {
		$annotation .= " $wrap_type";
	}
	if ($pkt_type eq "dU") {
		$annotation .= " ACK";
		if ($nack > 0x80) {
			$annotation .= ($seqnum + $nack - 1 - 0x100);
		} elsif ($nack < 0x80) {
			$annotation .= ($seqnum + $nack - 1);
		}
	}
	if ($pkt_type eq "dY") {
		$annotation .= " SYN";
	}
	if ($pkt_type eq "dF") {
		$annotation .= " FIN";
	}
	if ($pkt_type eq "dG") {
		$annotation .= " Gathered";
	}
	# Note packet number of outgoing reliable Activenet packets
	if ($tag eq "tx" and ($pkt_type =~ /d[TUYF]/)) {
		if ($pkt_type eq "dT") {
			$key = "${peer}_$seqnum";
			if (!exists($tx{$key})) {
				$tx{$key} = $time;
			} else {
				$tx{$key} = -1;
			}
		}
    }
	# Note round-trip time of incoming reliable Activenet pkts
	if ($tag eq "rx" and ($pkt_type =~ /d[TUYF]/)) {
		if ($pkt_type eq "dU") {
			$key = "${peer}_$seqnum";
			if (exists($tx{$key}) and $tx{$key} != -1) {
				$ping = $time - $tx{$key};
				$annotation .= " P$ping";
			}
		}
	}

	# B_ used by band.c for periodic unreliable broadcasts
	if ($tag eq "tx" and $pkt_type eq "B_") {
		$body =~ /^.. .. (.. .. .. ..) (.. .. .. ..)/;
		$pktnumhex = join "", (split /\s/, $2);
		$pktnum = unpack("L", pack("H*", $pktnumhex));
		$sent_idx = $pktnum % $SENT_LIST_LENGTH;
		$sent_time[$sent_idx] = $time;
		$sent_pktnum[$sent_idx] = $pktnum;
	# BR used by band.c for occasional replies to B_; contains latency info
	} elsif ($tag eq "rx" and $pkt_type eq "BR") {
		$body =~ /^.. .. (.. .. .. ..)/;
		$pktnumhex = join "", (split /\s/, $1);
		$pktnum = unpack("L", pack("H*", $pktnumhex));
		$sent_idx = $pktnum % $SENT_LIST_LENGTH;
		if ($sent_pktnum[$sent_idx] == $pktnum) {
			$ping = ($time - $sent_time[$sent_idx]);
			$hist_ping{int($ping / 10)}++;
		} else {
			$ping = "BIG";
			$hist_ping{9999}++;
		}
		$annotation .= " " if $annotation;
		$annotation .= "Ping $ping";
	}
	# BT used by band.c for sending test parameters
	if ($wrap_type eq "BT") {
		$wrap_body =~ /^(.. .. .. ..) (.. .. .. ..)/;
		$intervalhex = join "", (split /\s/, $1);
		$pktsizehex = join "", (split /\s/, $2);
		$interval = unpack("L", pack("H*", $intervalhex));
		$pktsize = unpack("L", pack("H*", $pktsizehex));
		$annotation .= " " if $annotation;
		$annotation .= "${pktsize}b/${interval}ms" if $interval;
		$annotation .= "${pktsize}b/adapt" if not $interval;
	}
    # AL used by new July 2 BZone for Ping Replies
	if ($tag eq "rx" and $pkt_type eq "AL") {          
		$body =~ /^.. .. (.. ..) (.. ..)/;
		$sendtimehex = join "", (split /\s/, $1);
		$maxpinghex = join "", (split /\s/, $2);
		$sendtime = unpack("S", pack("H*", $sendtimehex));
		$maxping = unpack("S", pack("H*", $maxpinghex));
		$ping = ($time - $sendtime) % 65536;
		$annotation .= " " if $annotation;
		$annotation .= "Ping $ping Max $maxping";
	}
	# Note packet loss relative to 2nd host if 2nd host's logfile was given
	# Kludge: only note packet loss of packets we know from tag are broadcast
	if (@ARGV > 1 and $tag eq "tx" and $pkt_type =~ /^(_|B_)/) {
		$binpacket = pack("H*", $body);
		if (!$rx{$binpacket}) {
			$annotation = "***packet lost***";
		}
	}
	# _ used by Battlezone for periodic unreliable status broadcast
	# B_ used by band.c for periodic unreliable status broadcast
	# Print out interval between adjacent incoming status packets
	if ($tag eq "rx" and $pkt_type =~ /^(_|B_)/) {
		# Print dt = Interval between adjacent packets from same host
		$dt = 0;
		if ($last_t[$peer] ne "") {
			$dt = $time -  $last_t[$peer];
		}
		$annotation = "dt $dt";
		# Print DT = Interval between adjacent packets from any host
		$dt = $time -  $last_t;
		if (($dt > 500) && ($last_t ne "")) {
			$annotation .= " DT $dt";
			$hist_DT{$dt}++;
		}

		$last_t[$peer] = $time;
		$last_t = $time;
    }
	# PB used by Battlezone for sending current allowed bandwidth
	if ($pkt_type eq "PB") {
		$body =~ /^.. .. (.. .. .. ..) (..) .*$/;
		$bwhex = join "", (split /\s/, $1);
		$bandwidth = unpack("L", pack("H*", $bwhex));
		if ($length == 16) {  # new packet with send interval byte
			$sendinterval = unpack("S", pack("H4", "${2}00")) * 4;
		} else {
			$sendinterval = -1;
		}
		$annotation .= " " if $annotation;
		$annotation .= "BW ${bandwidth}Bps";
		$annotation .= " Int ${sendinterval}ms" if $sendinterval != -1;
    }

	# Finally, print the annotation together with the original packet,
	# truncated to 80 characters for easy reading.
	$opline = sprintf("$pktinfo %3d : %s $body", $length, $pkt_type);
	printf("%-25.25s| %-52.52s\n", $annotation, $opline);	
	
	if ($pkt_type eq "dG") {
		$lenbody = length($body);
		$ilen = 6;  # skip past hex for two byte type (dG)
		while ($ilen < $lenbody - 1) {
			$next_head = substr($body, $ilen, 8);
			if ($next_head =~ /^(..) (..) (..)/) {
				$subpktlen = unpack("S", pack("H4", "${1}00"));
				$subpkttype = pack("H4", "$2$3"); 
				$subpktbody = substr($body, $ilen + 3, 3*$subpktlen - 1);

				&print_packet(" **** ", $tag, $subpkttype, $subpktbody, $pktinfo, $subpktlen);
			} else {
				die "failed to ungather: $ilen $body";
			}
			$ilen += 3 * $subpktlen + 3;
		}
	}
}
