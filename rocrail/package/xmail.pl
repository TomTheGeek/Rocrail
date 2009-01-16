#!/usr/bin/perl
#

require 5.000;

$mailaddr = $ARGV[0];

open(SENDMAIL, "| /usr/sbin/sendmail -t -n") || die "Unable to open sendmail";
print SENDMAIL "From: Rocrail\n";
print SENDMAIL "To: $mailaddr\n";
print SENDMAIL "Reply-To: Rocrail\n";
print SENDMAIL "Subject: Exception file created.\n";
print SENDMAIL "This email is sended to notify you because an exception file is created.\n";


close(SENDMAIL);
