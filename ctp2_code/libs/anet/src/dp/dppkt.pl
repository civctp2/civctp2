# Script to analyse output from various packet loggers.
# $Log: dppkt.pl $
# Revision 1.4  1997/09/28 03:53:12  dkegel
# Copes with new dp a little better
# Revision 1.3  1997/07/25 21:27:56  dkegel
# Added ping packet type; back to modern port number.
# Revision 1.2  1997/06/22 18:57:14  dkegel
# Added code to handle DOS dumps of UDP traffic.
# Revision 1.1  1997/06/02 01:23:55  dkegel
# Initial revision

sub dehex
{
	local($in, $out, $byte);
	$in = $_[0];
	while ($in ne "") {
		$byte = substr($in, 0, 2);
		substr($in, 0, 2) = "";
		$out .= sprintf("%c", hex($byte));
	}
	return $out;
}

# Usage: &analyse($src, $dest, $body)
# where $body is an ASCII string of two-digit hex bytes, no spaces
sub analyse
{
	local ($src, $dest, $ipxbody) = @_;
	
	# Analyse DP header
	($dtag1, $dtag2, $dbody) = unpack("a2 a2 a*", $ipxbody);
	
	$dtag1 = hex($dtag1);
	$dtag2 = hex($dtag2);
	$dtag = sprintf("%c%c", $dtag1, $dtag2);

	if ($dtag !~ /^d/) {
		return;
	}
	
	#print "cksum $xcksum len $xlen tc $xtc pt $xpt\n";
	print "dest $dest src $src tag $dtag body $dbody\n";
	
	# Analyse DP body
	if ($dtag eq "dY") {
		# syn
		($pktnum, $len, $ver, $myadrlen, $myadr) = unpack("a4 a2 a2 a2 a*", $dbody);
		print "syn:  pktnum $pktnum len $len ver $ver al $myadrlen myadr $myadr\n\n";
	} elsif ($dtag eq "dT") {
		# reliable data
		($pktnum, $len, $data) = unpack("a4 a2 a*", $dbody);
		print "data: pktnum $pktnum len $len data $data\n\n";
	} elsif ($dtag eq "dS") {
		# session advertisment

# Mercnet 1.05 fieldwidths
#	0  1  2 playerHdl_t	hMaster;
#	1  10 20 byte   		adrMaster[dp_MAX_ADR_LEN];	
#	11 2  4  dpid_t		masterPseudoplayer;
#	13 2  4  dp_species_t	sessionType;
#	15 2  4  dp_karma_t	karma;
#	17 2  4  short		maxPlayers;
#	19 2  4  short		currentPlayers;
#	21 2  4  short		flags;
#	23 32 64 char_t		sessionName[dp_SNAMELEN];
#	55 10 20 char		szUserField[dp_USERFIELDLEN];
#	65 15 30 char_t		szPassword[dp_PASSWORDLEN];
#	75 4  8  long		dwReserved1;
#	79 4  8  long		dwUser1;					

		($sdummy, $sadrmaster, $smasterpp, $stype, $skarma, $smaxp, $scurp, $sfl, $sname, $suserf, $spw, $sres, $suser1, $oops) = 
		#unpack("a2 a20         a4          a4      a4       a4      a4      a4    a32     a10       a10  a8     a8 a*", $dbody);
		unpack("a2 a20         a4          a4      a4       a4      a4      a4    a64     a20       a30  a8     a8 a*", $dbody);
		$sname = &dehex($sname);
		$spw = &dehex($spw);
		print "oops: $xoops\n" if ($xoops ne ""); 
		print "short\n" if ($suser1 eq ""); 
		print "sess: am $sadrmaster spe $stype karm $skarma max $smaxp cur $scurp fl $sfl name $sname, $suserf, $spw, $sres, $suser1\n\n";
	} elsif ($dtag eq "dA") {
		# add player
		($aid, $akarma, $aadr, $aname, $astype, $askarma) = 
		unpack("a4 a4   a20     a48     a4       a4", $dbody);
		$aname = &dehex($aname);
		print "addpl: id $aid karma $akarma adr $aadr name $aname stype $astype skarma $askarma\n\n";
	} elsif ($dtag eq "dB") {
		print "ping:\n";
	} elsif ($dtag eq "dC") {
		print "ping response:\n";
	}
}

# Analyse an Ethernet frame
# Usage: &ethernet_analyse($mactype, $macbody)
sub ethernet_analyse
{
	local($mactype, $macbody) = @_;
	local($src, $dest, $body);

	# This assumes Ethernet (not 802.3) encapsulation;
	# see "TCP/IP Illustrated", pg. 23
	if ($mactype eq "0800") {
		# It's IP
		# part after type is:
		# ver4 hdrlen
		# 450000711B930000401179C5CE4F0503CF9B423652A252A2005DFBAB6...
		$iphdr = substr($macbody, 0, 2*20);
		$ipbody = substr($macbody, 2*20);
		($iver, $ihdrlen, $itos, $ilen, $iid, $ifragoff, $ittl, $iproto, $ichk, $isrc, $idest) = unpack("a1 a1 a2 a4 a4 a4 a2 a2 a4 a8 a8", $iphdr);
		$udphdr = substr($ipbody, 0, 16);
		$udpbody = substr($ipbody, 16);
		($usrcport, $udestport, $ulen, $uchk) = unpack("a4 a4 a4 a4", $udphdr);
		#print "hdr: $iver, $ihdrlen, $itos, $ilen, $iid, $ifragoff, $ittl, $iproto, $ichk, $isrc, $idest\n";
		print "udphdr: srcport $usrcport, destport $udestport, $ulen, $uchk\n";
		#print "udpbody: $udpbody\n";

		# Make sure it's the right port; ignore other traffic
		#return if ($udestport ne "52A4");		# 52a1 = dos netmech, 52a2 = merc 1.06, 52a3 = I'76
		$body = $udpbody;
		$src = $isrc;
		$dest = $idest;
		#print "body: $body\n";

	} elsif ($mactype eq "8137") {
		# It's IPX
		# Strip SNAP(???)
		$snaphdr = substr($macbody, 2*0, 2*3);
		
		# Strip IPX header 
		$ipxhdr = substr($macbody, 2*3, 2*30);
		#print "ipxhdr $ipxhdr\n";
		
		$ipxbody = substr($macbody, 2*33);
		
		# Analyse IPX header
		($xcksum, $xlen, $xtc, $xpt, 
		 $xdestnet, $xdestnode, $xdestsock,
		 $xsrcnet, $xsrcnode, $xsrcsock, $xoops) = unpack("a4 a4 a2 a2 a8 a12 a4 a8 a12 a4 a1", $ipxhdr);
		print "oops: $xoops" if ($xoops ne ""); 
		#print "hmm: dest $macdest ne $xdestnode\n" if ($macdest ne $xdestnode);
		#print "hmm: src $macsrc ne $xsrcnode\n" if ($macsrc ne $xsrcnode);
		print "sock $xdestsock\n";
		
		# Make sure it's the right port; ignore other traffic
		return if ($xdestsock ne "52A2");		# 52a1 = dos netmech, 52a2 = merc 1.06, 52a3 = I'76
		$body = $ipxbody;
		$src = $xsrcnode;
		$dest = $xdestnode;
	} else {
		return;
	}

	&analyse($src, $dest, $body);
	print "\n";
}

if (1) {
	# Handle a DOS ETHLOAD file
	# Script to analyse output from ETHLOAD /r packet logger.
	# Ethload home page: http://www.ping.be/~pin01407/ethload.html
	# Assumes that you're monitoring IPX packets, and the whole Ethernet
	# packet is reported..
	#
	# Format:
	# ; comment
	# ticks_from_start size_in_bytes MAC_Destination MAC_Source Ethernet_Type Ethernet_Data
	# 0000000002 0061 FFFFFFFFFFFF 080007F7E3FD 002C E0-E0-03-FF-FF-00-28-00-01-00-00-00-00-FF-FF-FF-FF-FF-FF-04-53-00-00-00-00-08-00-07-F7-E3-FD-40-00-00-01-FF-FF-FF-FF-FF-FF-FF-FF-40-00-00-02
	#
	while (<>) {
		local ($ticks, $macsize, $macdest, $macsrc, $mactype, $macbody);
		next if /^;/;
		
		s/-//g;
		
		($ticks, $macsize, $macdest, $macsrc, $mactype, $macbody) = split(" ");
		next if ($macbody eq "");
		#print "mactype $mactype\n";
		#print "macbody $macbody\n";

		&ethernet_analyse($mactype, $macbody);
	}
} else {
	# Handle a Solaris snoop or Linux tcpdump file
	# Assumes you're monitoring UDP packets, and only the UDP payload is
	# reported.
	# Format: 
	# 13:05:16.914188 206.79.5.2.21154 > mercnet.activision.com.21154: udp 14
	# \t\t\t 0011 2233 4455
	# \t\t\t 6677 8899 aabb ...
	# Note that bytes are printed in the order they occur in the packet,
	# with a space added every other byte.  (No byte swapping required.)
	local ($time, $ipsrc, $ipdest, $iptype, $iplen, $ipbody);
	while (<>) {
		chop;
		if (/^\d/) {
			if ($ipbody ne "") {
				print "Anal $ipbody\n";
				&analyse($ipbody);
			}
			($time, $ipsrc, $ipdest, $iptype, $iplen) = split(" ");
			$ipbody = "";
		} elsif (/^\t/) {
			s/^\t\t\t//;
			s/\s//g;
			$ipbody .= $_;
		}
	}
}
