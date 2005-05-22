#!/usr/bin/perl
require 5.002;
use strict;
use Socket;

my %FORM = ();
my $command = "";
&Parse_Form;

if (!exists($FORM{'Action'}) || !exists($FORM{'Username'})
||  !exists($FORM{'Password'})) {
	&HTML_Error("Invalid Form");
}
if ($FORM{'Username'} !~ /^\S+$/) {
	&HTML_UserError("The username you entered is not valid.");
}
if ($FORM{'Password'} !~ /^\S+$/) {
	&HTML_UserError("The password you entered is not valid.");
}
if ($FORM{'Action'} =~ /^N/i) {
	if (!exists($FORM{'PasswordCheck'}) || !exists($FORM{'Email'})) {
		&HTML_Error("Invalid Form");
	}
	if ($FORM{'Email'} !~ /^\S+\@\S+\.[^.\s]+$/) {
		&HTML_UserError("The email address you entered is not valid.");
	}
	if ($FORM{'PasswordCheck'} ne $FORM{'Password'}) {
		&HTML_UserError("The passwords you entered do not match.");
	}
	$command = "N $FORM{Username} $FORM{Password} $FORM{Email}\n";
} elsif ($FORM{'Action'} =~ /^A/i) {
	if (!exists($FORM{'ActivationCode'})) {
		&HTML_Error("Invalid Form");
	}
	if ($FORM{'ActivationCode'} !~ /^\S+$/) {
		&HTML_UserError("The activation code you entered is not valid.");
	}
	$command = "A $FORM{Username} $FORM{Password} $FORM{ActivationCode}\n";
} elsif ($FORM{'Action'} =~ /^D/i) {
	$command = "D $FORM{Username} $FORM{Password}\n";
} elsif ($FORM{'Action'} =~ /^P/i) {
	if (!exists($FORM{'NewPassword'}) || !exists($FORM{'NewPasswordCheck'})) {
		&HTML_Error("Invalid Form");
	}
	if ($FORM{'NewPassword'} !~ /^\S+$/) {
		&HTML_UserError("The new password you entered is not valid.");
	}
	if ($FORM{'NewPasswordCheck'} ne $FORM{'NewPassword'}) {
		&HTML_UserError("The new passwords you entered do not match.");
	}
	$command = "P $FORM{Username} $FORM{Password} $FORM{NewPassword}\n";
} elsif ($FORM{'Action'} =~ /^E/i) {
	if (!exists($FORM{'NewEmail'})) {
		&HTML_Error("Invalid Form");
	}
	if ($FORM{'NewEmail'} !~ /^\S+\@\S+\.[^.\s]+$/) {
		&HTML_UserError("The new email address you entered is not valid.");
	}
	$command = "E $FORM{Username} $FORM{Password} $FORM{NewEmail}\n";
} else {
	&HTML_Error("Invalid Form");
}

socket(SOCK, PF_UNIX, SOCK_STREAM, 0)
	or HTML_Error("Socket Error: $!");
connect(SOCK, sockaddr_un("acctserv.so"))
	or HTML_Error("Connect Error: $!");
print SOCK "$command\n";
my $response = <SOCK>;
my ($status, $message);
if ($response =~ /^(\d+) (.*)$/) {
	$status = int($1);
	$message = $2;
} else {
	&HTML_Error("Server Error: Invalid Response");
}
if ($status == 0) {
	if ($FORM{'Action'} =~ /^A/i) {
		&HTML_Response($message,
		"Your account has now been activated, and should be available for ".
		"use on all Activision game servers within 10 minutes.<p>");
	} elsif ($FORM{'Action'} =~ /^P/i) {
		&HTML_Response($message,
		"Your password has been changed, and should propagate to all ".
		"Activision game servers within 10 minutes.<p>");
	} elsif ($FORM{'Action'} =~ /^D/i) {
		&HTML_Response($message,
		"Your account has been deleted, and should disappear from all ".
		"Activision game servers within 10 minutes.<p>");
	} else {
		&HTML_Error("Server Error: Invalid Response");
	}
} elsif ($status == 1) {
	if ($FORM{'Action'} =~ /^N/i) {
		&HTML_Response($message,
		"Your new account has been created.  You should receive an email ".
		"shortly containing the code you will need to activate your ".
		"new account.<p>");
	} elsif ($FORM{'Action'} =~ /^P/i) {
		&HTML_Response($message,
		"Your password has been changed.  You still need to activate your ".
		"account using the code in the email you received when you created ".
		"it.<p>");
	} elsif ($FORM{'Action'} =~ /^E/i) {
		&HTML_Response($message,
		"Your email address has been changed.  You should receive an email ".
		"shortly containing a new code you will need to reactivate your ".
		"account.<p>");
	} else {
		&HTML_Error("Server Error: Invalid Response");
	}
} elsif ($status == 2 || $status == 3) {
	&HTML_UserError("($status) $message");
} else {  /* $status < 0 || $status > 3 */
	&HTML_Error("($status) $message");
}
exit;


sub Parse_Form
{
	my ($buffer, $pair);
	read(STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
	my @pairs = split(/&/, $buffer);
	foreach $pair (@pairs) {
		my ($key, $val) = split(/=/, $pair);
		$key =~ tr/+/ /;
		$key =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
		$val =~ tr/+/ /;
		$val =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/eg;
		$FORM{$key} = $val;
	}
}

sub HTML_Error
{
	my ($message) = @_;
	&HTML_Header;
	print "<h3>Error: $message</h3>\n";
	print "Something went wrong during the processing of your request.<p>\n";
	print "Please try again later or report this problem to ",
	      "<i>address</i>.<p>\n";
	&HTML_Footer;
	exit;
}

sub HTML_UserError
{
	my ($message) = @_;
	&HTML_Header;
	print "<h3>Error: $message</h3>\n";
	print "Please return to the form and try again.<p>\n";
	&HTML_Footer;
	exit;
}

sub HTML_Response
{
	my ($title, $message) = @_;
	&HTML_Header;
	print "<h3>$title</h3>\n";
	print "$message\n";
	&HTML_Footer;
	exit;
}

sub HTML_Header
{
	print "Content-type: text/html\n\n";
	print "<html><head><title>Activision Account Server</title></head>\n";
	print "<body>\n";
}

sub HTML_Footer
{
	print "</body></html>\n";
}
