#!/usr/bin/perl
#create html page listing table of session type vs. game and
#crash logs according to date and session type
#WARNING: only works for current form of aehtab.in

if (@ARGV < 3) {
	die "expected args: webDir, typesfile, templatefile; all absolute paths";
}
$webdir = $ARGV[0];
$types  = $ARGV[1];
$template = $ARGV[2];

sub bynumber { $a <=> $b; }

#get list of all log files with date names and list of all directories with
#session type names for displaying in table form
chdir $webdir;
opendir(MERGLOG, ".");
@datelist = reverse sort grep(-f && /\d\d\-\d\d\-\d\d.bin$/, readdir(MERGLOG));
rewinddir MERGLOG;
foreach $sesstype (sort bynumber grep(-d && !/^\.\.?$/, readdir(MERGLOG))) {
    print "Opendir($webdir/$sesstype)...\n";
    opendir(SESSLOG, $webdir . "/" . $sesstype);
    if (grep(-f, readdir(SESSLOG))) {
		print "push $sesstype\n";
		push(@sesstype, $sesstype);
    }
    closedir SESSLOG;
}
closedir MERGLOG;

#calculate number of crashes for file $name in directory $dir and output to file
sub print_ninst {
    local($dir, $name, $line) = @_;
    opendir(LOCDIR, $dir);
    if ((@namelist = grep(/$name/, readdir(LOCDIR))) && grep(/$name.bin$/, @namelist)) {
        if (grep(/$name.txt/, @namelist)) {
            open(LOCFILE, $dir . "/" . $name . ".txt") || die "Can't open file: " . $dir . "/" . $name . ".txt\n";
            $ninst = 0;
            while (<LOCFILE>) {
                if (/Number of cases: (\d+)/) {
                    $ninst += $1;
                }
            }
            close LOCFILE;
        } else {
            $ninst = "";
        }
        $line =~ s/dpNINST/$ninst/g;
        print INDEXOUT $line;
    } else {
        print INDEXOUT "<td>-</td>\n";
    }
    closedir(LOCDIR);
}

open(INDEXIN, $template) || die "Can't open file: " . $template;
open(INDEXOUT, "> index.tmp") || die "Can't create file: index.tmp\n";
while(<INDEXIN>) {
    if (/^LS(.*\n)/) {
        push(@gamecmd, $1);
    } elsif (/^endLS/) {
        #print table of session type vs. games
        @gamebuf = @gamecmd;
        @sessbuf = @sesstype;
        open(TYPES, $types) || die "Can't open file: " . $types;
        while (<TYPES>) {
            if (/^(\S+)\s+\S+\s+(.*)/) {
                $sesstype = $1;
                $game{$sesstype} = $2;
            }
        }
        close TYPES;
        while (@sessbuf) {
            foreach $cmd (@gamebuf) {
                while ($cmd =~ /dpGAME/) {
                    if (@sessbuf) {
                        $sesstype = shift @sessbuf;
                        $game = defined $game{$sesstype} ? $game{$sesstype} : "unknown";
                        $cmd =~ s/dpSESSTYPE/$sesstype/;
                        $cmd =~ s/dpGAME/$game/;
                    } else {
                        $cmd =~ s/dpSESSTYPE//;
                        $cmd =~ s/dpGAME//;
                    }
                }
                print INDEXOUT $cmd;
            }
            @gamebuf = @gamecmd;
        }
    } elsif (/^LD(.*\n)/) {
        push(@datecmd, $1);
    } elsif (/^endLD/) {
        #print daily log files
        foreach $date (@datelist) {
            $date =~ s/.bin$//;
            @datebuf = @datecmd;
            foreach $cmd (@datebuf) {
                $cmd =~ s/dpDATE/$date/g;
                if($cmd =~ /dpSESSTYPE/) {
                    foreach $sesstype (@sesstype) {
                        $sessbuf = $cmd;
                        $sessbuf =~ s/dpSESSTYPE/$sesstype/g;
                        &print_ninst($sesstype, $date, $sessbuf);
                    }
                } else {
                    &print_ninst(".", $date, $cmd);
                }
                @datebuf = @datecmd;
            }
        }
    } elsif (/dpSESSTYPE/) {
        #print combined log files for session type
        $sessbuf = $_;
        foreach $sesstype (@sesstype) {
            $sessbuf1 = $sessbuf;
            $sessbuf1 =~ s/dpSESSTYPE/$sesstype/g;
            &print_ninst($sesstype, $sesstype, $sessbuf1);
        }
    } elsif (/dpALL/) {
        #print combined log file
        $_ =~ s/dpALL/all/g;
        &print_ninst(".", "all", $_);
    } elsif (/dpNSESSTYPE/) {
        $nsesstype = $#sesstype + 1;
        s/dpNSESSTYPE/$nsesstype/g;
        print INDEXOUT;
    } else {
        print INDEXOUT;
    }
}
close INDEXIN;
close INDEXOUT;
