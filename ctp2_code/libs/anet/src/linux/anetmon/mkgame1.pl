# Collects information from servstat.html files downloaded by
# DownloadGameServerFiles.class and writes it to games.html

#DownloadGameServerFiles saves index.html files as 0.html, 1.html, etc.;
#open them all and get game info
$html = "/tmp/0.html";
$i = 0;
while (-s $html) {
	open (HTML, $html) || die "Can't open file: " . $html . "\n";
	printf "processing " . $html . "\n";
	while (<HTML>) {
		if (/(\d+): (\d+) (\d+) (\d+)/) {
			$sesstype = $1;
			$curplayer = $2;
			$curgame = $3;
			$gameday = $4;

			$curplayer{$sesstype} += $curplayer;
			$curgame{$sesstype} += $curgame;
			$gameday{$sesstype} += $gameday;
		}
	}
	close HTML;
	$i += 1;
	$html = "/tmp/" . $i . ".html";
}

#initialize uninitialized game variables
foreach $sesstype (keys(%curplayer)) {
	if ($curplayer{$sesstype} eq "") {
		$curplayer{$sesstype} = 0;
	}
	if ($curgame{$sesstype} eq "") {
		$curgame{$sesstype} = 0;
	}
	if ($gameday{$sesstype} eq "") {
		$gameday{$sesstype} = 0;
	}
}

sub bynumber { $a <=> $b; }

#output info to game.html using format from game.in
open (GAME, "game1.in") || die "Can't open file: game1.in\n";
open (OUT, "> game1.html") || die "Can't open file: game1.html\n";
$time = `date`;
while (<GAME>) {
	if (/^GAMELIST sep=(.*) after=(.*)/) {
		$sep = $1;
		$after = $2;
		foreach $sesstype (sort bynumber keys(%curplayer)) {
			$after2 = $after;
			$after2 =~ s/\{CURPLAYER\}/$curplayer{$sesstype}/;
			$after2 =~ s/\{CURGAME\}/$curgame{$sesstype}/;
			$after2 =~ s/\{GAMEDAY\}/$gameday{$sesstype}/;
			if (($curplayer{$sesstype} != 0) ||
				($curgame{$sesstype} != 0) ||
				($gameday{$sesstype} != 0)) {
				push(@out, $sesstype . "<td>" . $after2)
			}
		}
		$_ = join($sep, @out);
	} elsif (/\{TIME\}/) {
		$_ =~ s/\{TIME\}/$time/;
	}
	print OUT;
}
close GAME;
close OUT;
