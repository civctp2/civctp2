open (META, "meta.in") || die "Can't open file: meta.in \n";
@meta = <META>;
close META;

open (TYPE, "types.txt") || die "Can't open file: types.txt \n";
$line = 0;
while (<TYPE>) {
	local($type, $url, $name, %xlate, $new_file, $key, $var);
	# parsing the string from the type.txt #
	if (!/(\S+)\s+(\S+)\s+(.+)\s*/) {
		print "Error: can't parse line $line in file: type.txt correctly\n";	
		next;
	}
	$type = $1;
	$url = $2;
	$name = $3;
	$name =~ s/\s+$//;
	$line++;
   
	$xlate {'dpTYPE'} = $type;
	$xlate {'dpURL'} = $url;
	$xlate {'dpNAME'} = $name;

	$new_file = $name.".tpl";
	$new_file =~ s/ /_/g;
	$new_file =~ s/[:'"()!]//g;
 
	# save the url for this game for later use
 	$url = $new_file; 
	$url =~ s/.tpl/.html/;
	$urls{$type} = $url;
	$types{$name} = $type;

	open(NEW_META_FILE, "> $new_file") || die "Can't create $new_file";
	foreach $meta (@meta) {
		$_ = $meta;
		if (/dp/) {
			foreach $key (keys(%xlate)) {
				$var = $xlate{$key};
				s/$key/$var/;
			}
		}
		print NEW_META_FILE;
	}
	close NEW_META_FILE;
}
close TYPE;

open (INDEX, "index.in") || die "Can't open file: index.in\n";
open (INDEXOUT, "> index.tpl") || die "Can't create file: index.tpl\n";
while (<INDEX>) {
	if (/^GAMELIST sep=(.*) after=(.*)/) {
		$sep = $1;
		$after = $2;
		foreach $name (sort(keys(%types))) {
			$after2 = $after;
			$after2 =~ s/\{GAMES\}/{GAMES $types{$name}}/;
			$after2 =~ s/\{GAMESDAY\}/{GAMESDAY $types{$name}}/;
			$after2 =~ s/\{GAMESWEEK\}/{GAMESWEEK $types{$name}}/;
			$after2 =~ s/\{CURGAMES\}/{CURGAMES $types{$name}}/;
			$after2 =~ s/\{CURPLAYERS\}/{CURPLAYERS $types{$name}}/;
			push(@out, "<A HREF=\"" . $urls{$types{$name}} . "\">" . $name . "</A>$after2\n");
		}
		$_ = join($sep, @out);
	}
	print INDEXOUT;
}
close INDEXOUT;
