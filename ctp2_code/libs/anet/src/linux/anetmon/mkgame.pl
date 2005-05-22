#!/usr/bin/perl
# Collects information from index.html files downloaded by
# DownloadGameServerFiles.class and writes it to games.html

# DownloadGameServerFiles save index.html files as prefix0.html,
# prefix1.html, etc.; open them all and get game info
if (@ARGV != 1) {
	print STDERR "Usage: perl mkgame.pl tempfilePrefix\n";
	print STDERR "   tempfilePrefix must be the same one passed to\n";
	print STDERR "   DownloadGameServerFile.class to get the files.\n";
	exit 1;
}
$tempfilePrefix = $ARGV[0];

$html = "/tmp/${tempfilePrefix}0.html";
$i = 0;
while (-s $html) {
	open (HTML, $html) || die "Can't open file: " . $html . "\n";
	printf "processing " . $html . "\n";
	while (<HTML>) {
		if (/\<A HREF=\"\S+\"\>(.*)\<\/A\>\<td\>(\d+)\<td\>(\d+)\<td\>(\d+)/) {
			$name = $1;
			$curplayer = $2;
			$curgame = $3;
			$gameday = $4;

			$curplayer{$name} += $curplayer;
			$curgame{$name} += $curgame;
			$gameday{$name} += $gameday;
		}
	}
	close HTML;
	$i += 1;
	$html = "/tmp/${tempfilePrefix}${i}.html";
}

#initialize uninitialized game variables
foreach $name (keys(%curplayer)) {
	if ($curplayer{$name} eq "") {
		$curplayer{$name} = 0;
	}
	if ($curgame{$name} eq "") {
		$curgame{$name} = 0;
	}
	if ($gameday{$name} eq "") {
		$gameday{$name} = 0;
	}
}

#output info to game.html using format from game.in
open (GAME, "game.in") || die "Can't open file: game.in\n";
open (OUT, "> game.html") || die "Can't open file: game.html\n";
$time = `date`;
while (<GAME>) {
	if (/^GAMELIST sep=(.*) after=(.*)/) {
		$sep = $1;
		$after = $2;
		foreach $name (sort keys(%curplayer)) {
			$after2 = $after;
			$after2 =~ s/\{CURPLAYER\}/$curplayer{$name}/;
			$after2 =~ s/\{CURGAME\}/$curgame{$name}/;
			$after2 =~ s/\{GAMEDAY\}/$gameday{$name}/;
			push(@out, $name . "<td>" . $after2)
		}
		$_ = join($sep, @out);
	} elsif (/\{TIME\}/) {
		$_ =~ s/\{TIME\}/$time/;
	}
	print OUT;
}
close GAME;
close OUT;
