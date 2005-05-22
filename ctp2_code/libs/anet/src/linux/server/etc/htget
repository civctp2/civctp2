#!/usr/bin/perl

##
## htget 1.1  Copyright 1995 Nate Sammons, All Rights Reserved
##
##   modified 4/23/96 by nate to use HTTP proxies.
##
## Given a URL and an optional local file name, retrieves the
## file and either writes it to the same named file in the
## current directory or to the file given on the command line.
##
## It prints the HTTP header information provided by the remote
## machine to STDOUT while running.
##

$timeout = 10;

## parse the command line (last arg is the URL)
&parse_commandline(splice(@ARGV, 0, $#ARGV));

## URL is always the last thing on the comandline.
$url = $ARGV[$#ARGV];

## set an alarm handler
$SIG{'ALRM'} = 'alarmhandler';

##
## get file information from the URL
##
$_ = $url;
/^http:\/\/([^\/]+)(.+)$/;
($machine, $port) = split(/:/, $1);
$port = '80' unless $port;
$file = $2;
@tmp = split(/\//, $file);
if (!$outfile) { $outfile = $tmp[$#tmp]; }

if (!$file || !$machine || !$outfile) { &cmdlineopts; exit 0; }

use Socket;

## set an alarm
alarm $timeout;

if ($use_proxy) {
  &connect_socket($proxy_machine, $proxy_port);
  }
else {
  &connect_socket($machine, $port);
  }

## reset the alarm
alarm 0;

## request the document
if ($use_proxy) {
  print S "GET $url HTTP/1.0\n",
          "Accept: */*\n",
          "User-Agent: htget/1.1\n\n";
  }
else {
  print S "GET $file HTTP/1.0\n",
          "Accept: */*\n",
          "User-Agent: htget/1.1\n\n";
  }

## read past the header
READING_HEADER:
while (!$gotheader) {
  chop($_ = <S>);
  
  @tmp = split(' ');
  if ($tmp[0] eq 'HTTP/1.0') {
    if ($tmp[1] eq '401') {
      while ((split(':'))[0] ne 'WWW-authenticate') { chop($_ = <S>); }
      @tmp = split(' ', (split(':', $_, 2))[1]);
      if ($tmp[0] ne 'basic') {
        print "The remote host requested an authentication of\n",
              "type \"$tmp[0]\" (only \"basic\" is supported).\n\n";
        exit 0;
        }
      $realm = substr($tmp[1], 7, (length($tmp[1]) -8));
      print STDERR "\nYou are not authorized to access this file.\n\n",
                   "Do you wish to authenticate? [Y,n] : ";
      chop($_ = <STDIN>);
      tr/A-Z/a-z/;
      if (substr($_, 0, 1) ne 'y') {
        while ($_ = <S>) {
          print;
          }
        exit 0;
        }
      
      print "  Enter a login name for $realm at $machine:$port\n",
            "  login: ";
      chop($login = <STDIN>);
      system("stty -echo");
      print "  password: ";
      chop($password = <STDIN>);
      system("stty echo");
      $encoded = &base64_encode("$login:$password");
      $encoded =~ s/\s*$//;
      
      close(S);
      
      if ($use_proxy) {
        &connect_socket($proxy_machine, $proxyport);
        print S "GET $url HTTP/1.0\n",
                "User-Agent: htget/1.1\n",
                "Authorization: Basic $encoded\n\n";
        }
      else {
        &connect_socket($machine, $port);
        print S "GET $file HTTP/1.0\n",
                "User-Agent: htget/1.1\n",
                "Authorization: Basic $encoded\n\n";
        }
      
      goto READING_HEADER;
      }
    elsif ($tmp[1] ne '200') {
      print STDERR "\nServer $machine replied:\n";
      $no_write = 1;
      }
    }
  
  elsif ($tmp[0] eq 'Content-length:') {
    $content_length = $tmp[1];
    }
  if ($no_write) { print STDERR "  $_\n" };
  if (/^\s*$/) { $gotheader = 1; }
  }

## print some headers
if (!$no_write && !$use_pager) {
  print STDERR "  $url\n";
  
  if ($content_length) {
    print STDERR "  File size:    ", sprintf("%10d", $content_length),
                 " [ $outfile ]\n";
    }
  else { 
    print STDERR "  File size:       UNKNOWN [ $outfile ]\n";
    }

  print STDERR '  Reading data: ', ' ' x 10 unless $use_pager;
  if ($content_length) {
    print STDERR ' bytes, ', '       %';
    $bs = length(' ' x 10 . ' bytes, ' . '       %');
    }
  else {
    print STDERR ' bytes.';
    $bs = length(' ' x 10 . ' bytes.');
    }
  }

if (!$no_write) {
  if (!$use_pager) {
    open(OUT, ">$outfile");
    while ($buf = <S>) {
      $read += length($buf);
      print OUT $buf;
      if ($content_length) {
        print STDERR "\b" x $bs,
                     sprintf("%10d", $read), ' bytes, ',
                     sprintf("%6.2f", (($read/$content_length) * 100)), ' %';
        }
      else {
        print STDERR "\b" x $bs,
                     sprintf("%10d", $read), ' bytes.';
        }
      }
    close(OUT);
    $now = time;
    print STDERR "\n  Transfer successful, elapsed time was ",
                 ($now - $^T), " seconds.\n";
    }
  else {
    open(PAGER, "| $pager");
    while (<S>) {
      print PAGER;
      }
    close(PAGER);
    }
  }
else {
  print STDERR "  Transfer unsuccessful.\n\n",
        "  If you asked for a directory, you may want to try and\n",
        "  retrieve the file index.html from inside that directory.\n";
  
  if ($verbose) {
    print "\nServer replied:\n\n";
    while (<S>) { print; }
    print "\n\n";
    }
  
  }

close(S);
exit 0;

##########################################################################
##########################################################################

BEGIN {
  ##
  ## command line options, and functions associated with them,
  ## also, how many options to give them, and if we should
  ## exit after running the command.
  ##
  ##
  ## %cl   = the perl command to run
  ## %clo  = how many options does it take
  ## %clh  = a little help
  ## %cle  = exit after running? (0, 1)
  ## %cld  = what are the options that go with it?
  ##
  $cl{'-h'}          = '&cmdlineopts';
  $clo{'-h'}         = 0;
  $clh{'-h'}         = 'Prints command line options help';
  $cle{'-h'}         = 1;
  $cld{'-h'}         = 0;
  
  $cl{'-m'}          = '&set_use_pager';
  $clo{'-m'}         = 0;
  $clh{'-m'}         = 'Use a pager to view the file';
  $cle{'-m'}         = 0;
  $cld{'-m'}         = 0;
  
  $cl{'-q'}          = '&set_quiet';
  $clo{'-q'}         = 0;
  $clh{'-q'}         = 'Do not print any output';
  $cle{'-q'}         = 0;
  $cld{'-q'}         = 0;
  
  $cl{'-o'}          = '&set_outfile';
  $clo{'-o'}         = 1;
  $clh{'-o'}         = 'Set the output filename';
  $cle{'-o'}         = 0;
  $cld{'-o'}         = 'filename';
  
  $cl{'-v'}          = '&set_verbose';
  $clo{'-v'}         = 0;
  $clh{'-v'}         = 'Print verbose output';
  $cle{'-v'}         = 0;
  $cld{'-v'}         = 0;

  $cl{'-t'}          = '&set_timeout';
  $clo{'-t'}         = 1;
  $clh{'-t'}         = 'Sets the timeout in seconds (default=10)';
  $cle{'-t'}         = 0;
  $cld{'-t'}         = 'seconds';

  $cl{'-p'}          = '&set_proxy';
  $clo{'-p'}         = 1;
  $clh{'-p'}         = 'Sets the proxy connection to use';
  $cle{'-p'}         = 0;
  $cld{'-p'}         = 'machine[,port] (port=80 by default)';

  ##
  ## Base 64 decoding/encoding stuff
  ##
  $base64_alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'.
               	 'abcdefghijklmnopqrstuvwxyz'.
               	 '0123456789+/';
  $base64_pad = '=';

  $uuencode_alphabet = q|`!"#$%&'()*+,-./0123456789:;<=>?|.
                        '@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_'; # double that '\\'!
  $uuencode_pad = '`';

  # Build some strings for use in tr/// commands.
  # Some uuencodes use " " and some use "`", so we handle both.
  # We also need to protect backslashes and other special characters.
  $tr_uuencode = " ".$uuencode_alphabet;
  $tr_uuencode =~ s/(\W)/\\$1/g;
  $tr_base64 = "A".$base64_alphabet;
  $tr_base64 =~ s/(\W)/\\$1/g;
  }

##
## Connect a socket to a remote machine and port
##
sub connect_socket {
  my($machine, $port) = @_;
  my($sockaddr) = 'S n a4 x8';
  chop($localhost = `hostname`);
  my($name, $aliases, $proto) = getprotobyname('tcp');
  my($name1, $aliases1, $type1, $len1, $localaddr) = gethostbyname($localhost);
  my($name2, $aliases2, $type2, $len2, $remaddr) = gethostbyname($machine);
  my($local_pack)  = pack($sockaddr, &AF_INET, 0, $localaddr);
  my($remote_pack) = pack($sockaddr, &AF_INET, $port, $remaddr);
  socket(S, &PF_INET, &SOCK_STREAM, $proto) || die "Socket: $!";
  bind(S, $local_pack) || die "Bind: $!";
  connect(S, $remote_pack) || die "Connect: $!";
  select(S); $| = 1; select(STDOUT);
  }

##
## Parse command line options
##
sub parse_commandline {

  local(@args) = @_;
  local(@opts, $i, $o, $j);
  
  ##
  ## command line options, and functions associated with them,
  ## also, how many options to give them, and if we should
  ## exit after running the command.
  ##
  ## interpret command line options and do sanity checking on them
  ##
  for ($i = 0 ; $i <= $#args ; ) {

    ## if the option is defined,
    if (defined $clo{$args[$i]}) {
	 $o = $args[$i];
	 $j = $i + $clo{$args[$i]};
	 undef @opts;

	 ## assemble the options to the command,
	 for ($i++ ; $i < $j + 1 ; $i++) {
        if ($i > $#args || (defined $cl{$args[$i]})) {
     	print "Option '$o' usage: \"$o $cld{$o}\"\n",
          	 "Use the '-h' option for help.\n\n";
     	exit 0;
     	}
        push(@opts, $args[$i]);
        }

	 ## and pass them to the evaluator.
	 eval "$cl{$o}('" . join("', '", @opts) . "')";
	 if ($cle{$o}) { exit 0; }
	 }

    ## if the option is not defined, tell them and exit.
    else {
	 print "\n$name $version\n\n",
            "The option '$args[$i]' is not supported.\n\n",
            "Call to $0 was:\n",
            '  ', join(' ', @args), "\n\n";

	 ## print a list of actual options
	 &cmdlineopts;
	 exit 0;
	 }
    }

  }

##
## prints command line options
##
sub cmdlineopts {
  local($space) = 5;

  print "\n",
	"Usage:  $0 [options] URL\n\n",
	"Options are as follows:\n";

  foreach $i (sort keys %cl) {
    print ' ', &nprint($i, $space, ' '), '  ', $clh{$i}, "\n";
    if ($cld{$i}) {
      print ' ' x ($space +4), 'Usage: ', $i, ' ', $cld{$i}, "\n";
      }
    }
  print "\n";
  }

##
## Modified pretty printer
##
sub nprint {
  local($text, $space, $char) = @_;
  $text = $text . $char x ($space - length($text));
  }

##
## Use a pager to view the output file (does not write to a file)
##
sub set_use_pager {
  if ($quiet) {
    open(STDOUT, '>-');
    print STDOUT "The -m flag is incompatible with the -q flag.\n";
    exit 0;
    }
  $use_pager = 1;
  $pager = $ENV{'PAGER'} || 'more';
  }

##
## Verbose output
##
sub set_verbose { $verbose = 1; }

##
## Run quietly.
##
sub set_quiet {
  $quiet = 1;
  if ($use_pager) {
    print STDOUT "The -q flag is incompatible with the -m flag.\n";
    exit 0;
    }
  close(STDOUT);
  close(STDERR);
  }

##
## Set the output filename
##
sub set_outfile {
  $outfile = shift;
  }

#########################################################################
#########################################################################
# base64.pl -- A perl package to handle MIME-style BASE64 encoding
# A. P. Barrett <barrett@ee.und.ac.za>, October 1993
# $Revision: 1.4 $$Date: 1994/08/11 16:08:51 $

# Synopsis:
#       require 'base64.pl';
#
#       $uuencode_string = &base64'b64touu($base64_string);
#       $binary_string = &base64'b64decode($base64_string);
#       $base64_string = &base64'uutob64($uuencode_string);
#       $base64_string = &base64'b64encode($binary_string);
#       $uuencode_string = &base64'uuencode($binary_string);
#       $binary_string = &base64'uudecode($uuencode_string);
#
#       uuencode and base64 input strings may contain multiple lines,
#       but may not contain any headers or trailers.  (For uuencode,
#       remove the begin and end lines, and for base64, remove the MIME
#       headers and boundaries.)
#
#       uuencode and base64 output strings will be contain multiple
#       lines if appropriate, but will not contain any headers or
#       trailers.  (For uuencode, add the "begin" line and the
#       " \nend\n" afterwards, and for base64, add any MIME stuff
#       afterwards.)

####################

sub base64_encode {
  local($data) = shift;
  &uutob64(pack("u*", $data));
  }

sub base64_decode {
  local($data) = shift;
  unpack("u*", &b64touu($data));
  }

sub b64touu
{
    local ($_) = @_;
    local ($result);
    
    # zap bad characters and translate others to uuencode alphabet
    eval qq{
	tr|$tr_base64||cd;
	tr|$tr_base64|$tr_uuencode|;
    };

    # break into lines of 60 encoded chars, prepending "M" for uuencode
    while (s/^(.{60})//) {
	$result .= "M" . $& . "\n";
    }

    # any leftover chars go onto a shorter line
    # with padding to the next multiple of 4 chars
    if ($_ ne "") {
	$result .= substr($uuencode_alphabet, length($_)*3/4, 1)
		   . $_
		   . ($uuencode_pad x ((60 - length($_)) % 4)) . "\n";
    }

    # return result
    $result;
}

sub b64decode
{
    local ($_) = @_;
    local ($result);
    
    # zap bad characters and translate others to uuencode alphabet
    eval qq{
	tr|$tr_base64||cd;
	tr|$tr_base64|$tr_uuencode|;
    };

    # break into lines of 60 encoded chars, prepending "M" for uuencode,
    # and then using perl's builtin uudecoder to convert to binary.
    while (s/^(.{60})//) {
	#warn "chunk :$&:\n";
	$result .= unpack("u", "M" . $&);
    }

    # also decode any leftover chars
    if ($_ ne "") {
	#warn "last chunk :$_:\n";
	$result .= unpack("u",
		    substr($uuencode_alphabet, length($_)*3/4, 1) . $_);
    }

    # return result
    $result;
}

sub uutob64
{
    local ($_) = @_;
    local ($result);
    
    # This is the most difficult, because some perverse uuencoder
    # might have made lines that do not describe multiples of 3 bytes.
    # I don't see any better method than uudecoding to binary and then
    # b64encoding the binary.

    &b64encode(&uudecode); # implicitly pass @_ to &uudecode
}

sub b64encode
{
    local ($_) = @_;
    local ($chunk);
    local ($result);
    
    # break into chunks of 45 input chars, use perl's builtin
    # uuencoder to convert each chunk to uuencode format,
    # then kill the leading "M", translate to the base64 alphabet,
    # and finally append a newline.
    while (s/^((.|\n){45})//) {
	#warn "in:$&:\n";
	$chunk = substr(pack("u", $&), $[+1, 60);
	#warn "packed    :$chunk:\n";
	eval qq{
	    \$chunk =~ tr|$tr_uuencode|$tr_base64|;
	};
	#warn "translated:$chunk:\n";
	$result .= $chunk . "\n";
    }

    # any leftover chars go onto a shorter line
    # with uuencode padding converted to base64 padding
    if ($_ ne "") {
	#warn "length ".length($_)." \$_:$_:\n";
	#warn "enclen ", int((length($_)+2)/3)*4 - (45-length($_))%3, "\n";
	$chunk = substr(pack("u", $_), $[+1,
			int((length($_)+2)/3)*4 - (45-length($_))%3);
	#warn "chunk:$chunk:\n";
	eval qq{
	    \$chunk =~ tr|$tr_uuencode|$tr_base64|;
	};
	#warn "translated:$chunk:\n";
	$result .= $chunk . ($base64_pad x ((60 - length($chunk)) % 4)) . "\n";
    }

    # return result
    $result;
}

sub uuencode
{
    local ($_) = @_;
    local ($result);
    
    # break into chunks of 45 input chars, and use perl's builtin
    # uuencoder to convert each chunk to uuencode format.
    # (newline is added by builtin uuencoder.)
    while (s/^((.|\n){45})//) {
	$result .= pack("u", $&);
    }

    # any leftover chars go onto a shorter line
    # with padding to the next multiple of 4 chars
    if ($_ ne "") {
	$result .= pack("u", $_);
    }

    # return result
    $result;
}

sub uudecode
{
    local ($_) = @_;
    local ($result);
    
    # use perl's builtin uudecoder to convert each line
    while (s/^([^\n]+\n?)//) {
	$result .= unpack("u", $&);
    }

    # return result
    $result;
}

sub alarmhandler {
  die "Connection timed out after $timeout seconds\n";
  }

sub set_timeout {
  $timeout = shift;
  }

sub set_proxy {
  my($proxy) = shift;
  ($proxy_machine, $proxy_port) = split(',', $proxy);
  $proxy_port = 80 unless $proxy_port;
  $use_proxy = 1;
  }
