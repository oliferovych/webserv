#!/usr/bin/env perl

use strict;
use warnings;
use CGI;
use POSIX qw(strftime);

my $cgi = CGI->new;

print $cgi->header(-type => 'text/html', -charset => 'UTF-8', -method => 'POST');

print "<html>";
print "<head>";
print "<title>Current Time</title>";
print "<style>";
print "body { font-family: Arial, sans-serif; background-color: #f0f0f0; text-align: center; padding-top: 50px; }";
print ".time-container { background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); display: inline-block; }";
print ".time { font-size: 2em; color: #333; }";
print "</style>";
print "</head>";
print "<body>";
print "<div class='time-container'>";
print "<h1>Current Time</h1>";
print "<p class='time'>" . strftime("%Y-%m-%d %H:%M:%S", localtime) . "</p>";
print "</div>";
print "</body>";
print "</html>";
