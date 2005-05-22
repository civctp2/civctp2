#!/bin/sh
# Download the crash files from all servers for a date ($1),
# merge them into $CRASH_WEBHOME/$Date.{bin,txt},
# and split them out by day and by game into
# $CRASH_WEBHOME/$sessType/$Date.{bin,txt}.
set -x
set -e

. ~/crash/crashenv
javaDir=/usr/local/jdk
classPath=$javaDir/lib/classes.zip:.

cd $CRASH_HOME

echo aeh.activision.com aeh.activision.com > crashservers.txt
cat servers.txt >> crashservers.txt

# Download date's crash files into /tmp/0.bin, etc.
Date=$1
$javaDir/bin/javac DownloadGameServerFile.java
$javaDir/bin/java -classpath $classPath DownloadGameServerFile -s crashservers.txt -f anet2/atvilog/$Date.bin -p onedatetmp$$.

# Set s to a string containing all crash log files so downloaded
s="/tmp/onedatetmp$$.*"
if [ ! -f /tmp/onedatetmp$$.0.bin ]; then
	echo No crash files downloaded.
	exit 1
fi

# Given an option string, an output .bin, and a bunch of input .bin's,
# munge together the input .bin's into the output .bin, 
# then filter the output bin and convert it to a .txt file.
dofile() {
    echo ------ dofile $*  ------
	df_options=$1;
	df_outbin=$2;
	shift
	shift
	df_inbin=$*

    if [ "$df_inbin" = "" ]; then
		echo No input files for $df_outbin!
		return
	fi

	# Merge the input files; include everything.
	$CRASH_HOME/procserv $df_options -q -o $df_outbin -sN -sM -sA $df_inbin

	# Filter out records users don't want to see, then convert to text.
	df_outtmp=`echo $df_outbin | sed s/.bin$/.tmp/`
	$CRASH_HOME/procserv -q -dS -dM -dE -dY -iF -iA -o $df_outtmp -sT -sV -sN -sM -sA $df_outbin
	if [ -s $df_outtmp ]; then
		df_outtxt=`echo $df_outbin | sed s/.bin$/.txt/`
		if $CRASH_HOME/analcrsh -q -m $CRASH_HOME/maps/modcrc.txt -o $df_outtxt $df_outtmp ; then
			true
		else
			echo "analcrsh failed"
			exit 1
		fi
		if [ ! -s $df_outtxt -a -f $df_outtxt ]; then
			rm -f $df_outtxt
		fi
	fi
	rm -f $df_outtmp
    echo ----- dofile done ------
}

# Merge the log files for today into $Date.bin; include everything.
a=$CRASH_WEBHOME/$Date.bin
dofile "" $a $s 
rm $s
if [ ! -s $a ]; then
	rm -f $a
	echo No crash logs found for today.
	exit 1
fi

# For all session types, generate a $Date.bin and $Date.txt
# with just that session type's info in a subdirectory named
# for the session type.
t=`awk '{printf("%d\n", $1)}' $CRASH_HOME/types.txt`
echo Session types = $t
for type in $t; do
	d=$CRASH_WEBHOME/$type
	if [ ! -d $d ]; then
		echo creating $d
		mkdir $d
	fi

	# Break out this session type's crashes for today
	dofile "-t $type" $d/$Date.bin $a

	# Find all the $Date.bin files for this session type
	g=`find $d -type f -name \*.bin -not -name "$type.bin" -maxdepth 1 -printf "$d/%f "`
	if [ -n "$g" ]; then
		dofile "-t $type" $d/$type.bin $g
	fi
done
