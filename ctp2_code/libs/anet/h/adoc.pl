# Grab design comments and their associated structures or function
# definitions from a .h file.
#
# $log$
#
# Design comments look like this:
# 
# /*--------------------------------------------------------------------------
#  Change the .INI file accessed by future calls.
#  Argument must be a pointer to a persistant string; no copy will be made
#  of the string contents.
# --------------------------------------------------------------------------*/
# DP_API void dpini_SetFile(char *iniFileName);
#
# The output should look like this:
#
# <a name="dpini_SetFile"><h3>dpini_SetFile</h3></a>
# <tt>DP_API void dpini_SetFile(char *iniFileName);</tt>
# <br>
# Change the .INI file accessed by future calls.
# Argument must be a pointer to a persistant string; no copy will be made
# of the string contents.

while (<>) {
	# preprocessor: skip anything from #ifndef dp_ANET2 to next #endif
	if ($skipstate == 0) {
		# start skipping if we see a #ifndef dp_ANET2
		$skipstate = 1 if (/^#ifndef dp_ANET2/);
	} else {
		# skip until and including next #endif
		# (yes, this is a kludge. It assumes conditionals are not nested.)
		$skipstate = 0 if (/^#endif/);
		next;
	}
	# preprocessor #2: turn on hyperlinks
	s/htm#/H--T--M#/g;
	s/([A-Za-z\.\/_]+\.H--T--M#[A-Za-z_]+)/<a href="\1">\1<\/a>/g;
	s/([A-Za-z\.\/_]+\.htm)/<a href="\1">\1<\/a>/g;
	s/H--T--M#/htm#/g;
	if ($state == 0) {
		# Skip until start of design comment
		$state = 1 if (/\/\*----------/);
		$c = "";
		$f = "";
	} elsif ($state == 1) {
		# Read until end of comment 
		if (/-----------\*\//) {
			$state = 2;
		} else {
			if (/^\s*([A-Z]|dp_RES_)/ && ($c ne "") && ($c !~ /<p>$/)) {
				# Capital letters or dp_RES_ at the beginning of lines start new lines
				$c .= "<br>";
			} elsif (/^$/ && ($c ne "")) {
				$c .= "<p>";
			}
			$c .= $_;
		}
	} elsif ($state == 2) {
		# Read until end of function definition 
		if ($f eq "") {
			# Check to make sure it's a function
			if (/^#/ || !/\(/) {
				# Nope, give up on it.
				$state = 0;
				next;
			}
		}
		$f .= $_;
		if (/;/) {
			my($fn_name) = "";
			if ($f =~ /.*?(\w+)\(/) {
				$fn_name = $1;
				# If function name ends in W or A, go on to read 2nd function.
				if ($fn_name =~ /[AW]$/) {
					$state = 3;
				} else {
					#print "--------\nn: $fn_name\nf:\n$f\nc:\n$c\n";
					$fns{$fn_name} .= "<a name=\"$fn_name\"><h3>$fn_name</h3></a>\n<pre>$f</pre>\n$c\n";
					$state = 0;
				}
			}
		}
	} elsif ($state == 3) {
		# Read until end of 2nd function definition 
		$f .= $_;
		if (/;/) {
			my($fn_name) = "";
			if ($f =~ /.*?(\w+)\(/) {
				$fn_name = $1;
				#print "--------\nn: $fn_name\nf:\n$f\nc:\n$c\n";
				$fn_name =~ s/[AW]$//;
				$fn_nameA = $fn_name . "A";
				$fn_nameW = $fn_name . "W";
				$fns{$fn_name} .= 
				  "<a name=\"$fn_name\"><h3>$fn_name, $fn_nameA, $fn_nameW</h3></a>\n"
				. "<pre>$f</pre>\n$c\n";
				$state = 0;
			}
		}
	}
}

foreach (sort(keys(%fns))) {
	print $fns{$_};
}
