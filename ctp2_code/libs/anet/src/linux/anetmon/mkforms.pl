#!/usr/bin/perl
open (SERVERIN, "servers.txt") || die "Can't open file servers.txt\n";
while (<SERVERIN>) {
   if (!/\S+\s+(\S+)\s*/) {
      print "Error: can't parse \"$line\" from servers.txt";
      next;
   }
   push(@server, $1);
}
close SERVERIN;
open (HTMLTPL, "runsrvfil.in") || die "Can't open file runsrvfil.in\n";
open(HTMLOUT, "> runsrvfil.html") || die "Can't create file runsrvfil.html\n";
while (<HTMLTPL>) {
   if (/^LF(.*\n)/) {
      push(@formcmd, $1);
   } elsif (/^endLF/) {
      foreach $server (@server) {
         @formbuf = @formcmd;
		 $servname = $server;
		 $servname =~ s/\..*//;
         foreach $cmd (@formbuf) {
            $cmd =~ s/dpSERVER/$server/g;
			$cmd =~ s/dpSERVNAME/$servname/g;
            print HTMLOUT $cmd;
         }
     } 
   } else {
      print HTMLOUT;
   }
}
close HTMLTPL;
close HTMLOUT;
