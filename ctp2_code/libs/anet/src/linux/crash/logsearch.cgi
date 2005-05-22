#!/usr/bin/perl

&Parse_Form;

if ($FORM{'crc'}) {
	$FORM{'crc'} =~ s/\\/\\\\/g;
	$FORM{'crc'} =~ s/'/\\'/g;
	$FORM{'crc'} =~ tr/a-f/A-F/;
	open(FIND, "find logs -name '*$FORM{crc}*.txt' -print |") or &HTML_Error("Find CRC error\n");
	&HTML_Header("Search results for crc=$FORM{crc}");
	while ($file = <FIND>) {
		chop $file;
		print "<a href=$file>$file</a><br>\n";
	}
	close(FIND);
	&HTML_Footer;
} elsif ($FORM{'grep'}) {
	$FORM{'grep'} =~ s/\\/\\\\/g;
	$FORM{'grep'} =~ s/'/\\'/g;
	open(FIND, "find logs -name '*.txt' -print | xargs -r grep -il '$FORM{grep}' |") or &HTML_Error("Grep error\n");
	&HTML_Header("Search results for grep=$FORM{grep}");
	while ($file = <FIND>) {
		chop $file;
		print "<a href=$file>$file</a><br>\n";
	}
	close(FIND);
	&HTML_Footer;
} else {
	&HTML_Error("Usage: $0?crc=xxxx or $0?grep=xxxx");
}
exit(0);

sub Parse_Form
{
    read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
	$buffer .= $ENV{'QUERY_STRING'};
    @pairs = split(/&/, $buffer);
    foreach $pair (@pairs) {
		($val1, $val2) = split(/=/, $pair);
		$val1 =~ tr/+/ /;
		$val1 =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
		$val2 =~ tr/+/ /;
		$val2 =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
		$val2 =~ s/\s+/ /g;
		$val2 =~ s/^\s+//g;
		$val2 =~ s/\s+$//g;
		if ($FORM{$val1}) {
		    $FORM{$val1} = "$FORM{$val1}, $val2";
		} else {
	    	$FORM{$val1} = $val2;
		}
	}
}

sub HTML_Header
{
	my ($title) = @_;
	print "Content-type: text/html\n\n";
	print "<html><head><title>$title</title></head>\n";
	print "<body>\n";
	print "<h3>$title</h3>\n";
}

sub HTML_Footer
{
	print "</body></html>\n";
}

sub HTML_Error
{
	my ($errmsg) = @_;
	print "Content-type: text/html\n\n";
	print "<html><head><title>Script Error</title></head>\n";
	print "<body>\n";
	print "<h3>$errmsg</h3>\n";
	print "</body></html>\n";
	exit(1);
}
