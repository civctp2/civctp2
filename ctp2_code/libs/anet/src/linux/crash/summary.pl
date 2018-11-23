#!/bin/perl
foreach $file (@ARGV) {
	my $comment, $state;
	my $adr, $moff, $foff, $dll, $sym, $path;
	my $ostr;
	my $count;
	$count = 0;
	$state = 0;
	$comment = "";
	$dll = "";
	$path = "";
	$sym = "";
	open(FILE, $file) || die "Can't open $file";
	while (<FILE>) {
		if (/^\*/) {
			$state = 0;
		}
		if ($state == 0) {
			# state 0 is the crash header, terminated by a line containing abs_addr
			if (/User Comment:(.*)/) {
				$comment .= $1 . "\n";
			} elsif (/Number of cases: (\d+)/) {
				$count += $1;
			} elsif (/abs_addr/) {
				$state++;
			}
		} elsif ($state == 1) {
			# state 1 is the stack traceback, terminated by an empty line
			if (!/\(addr too large/ && !/\(unknown/) {
				#($adr, $moff, $foff, $dll, $sym, $path) = split;
				# 402dc6     2dc6      156  HEAVY GEAR 2.EXE  _WinMainCRTStartup  maps/hg2/124.0
				/^\s*(\w+)\s+(\w+)\s+(\w+)\s+(\w.+\w)\s+(\S+)\s+(\S+)$/ || next;
				$adr = $1;
				$moff = $2;
				$foff = $3;
				$dll = $4;
				$sym = $5;
				$path = $6;
				if ($path ne "") {
					# state 2 is still stack traceback, but we don't care,
					# because we already know the function and dll of the crash.
					$state++;
				}   
			}
		}
	}
	close(FILE);
	$sym =~ s/&/&amp;/g;
	$sym =~ s/</&lt;/g;
	$comment =~ s/&/&amp;/g;
	$comment =~ s/</&lt;/g;
	$comment =~ s/\n+$//;
	@comment = split(/\n/,$comment);
	if (@comment) {
		$comment = "<ul><li>".join("\n<li>", @comment)."\n</ul>\n";
	} else {
		$comment = "<p>";
	}
	$ostr = "<h3><A HREF=\"$file\">$file</A> ($count occurences) ($path/$dll)</h3>
<code>0x$foff + $sym</code>
$comment
<HR>
";
	$index = sprintf("%s|%s|%8s", $dll, $sym, $foff);
	$out{$index} .= $ostr;
}
$dll = "";
foreach (sort(keys(%out))) {
	$ostr = $out{$_};
	($newdll, $sym, $foff) = split(/\|/);
	if ($newdll ne $dll) {
		$dll = $newdll;
		$ofile = $dll;
		$ofile =~ tr/A-Z/a-z/;
		$ofile =~ s/\W/_/g;
		$ofile .= ".html";
		open(OFILE, ">$ofile") || die "Can't create $ofile";
	}
	print OFILE $ostr;
}
