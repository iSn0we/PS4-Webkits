#!/usr/bin/perl -w

use CGI;
use HTTP::Date;

my $query = new CGI;
@names = $query->param;
my $includeBody = $query->param('include-body') || 0;
my $expiresInFutureIn304 = $query->param('expires-in-future-in-304') || 0;

# Parameter `return-200-at-first-response` means "Even if any other status code is requested, always return 200 at first time".
# Then, we can test as following sequence:
#  generate-response.cgi?status=304&return-200-at-first-response=1 (at 1st time)
#  -> returns 200, not 304, so this response is cached.
#  generate-response.cgi?status=304&return-200-at-first-response=1 (at 2nd time)
#  -> returns 304, and we can test conditional request
my $return200AtFirstResponse = $query->param('return-200-at-first-response') || 0;

my $uniqueId = $query->param('uniqueId') || 0;

# requestedCount is got from cookie. The 1st request has no cookie, so requestedCount = 0.
my $requestedCount = $query->cookie($uniqueId) || 0;

my $hasStatusCode = 0;
my $hasExpiresHeader = 0;

if ($return200AtFirstResponse && ($requestedCount == 0)) {
    print "Status: 200\n";
    $hasStatusCode = 1;
} elsif ($query->http && $query->http("If-None-Match") eq "match") {
    print "Status: 304\n";
    $hasStatusCode = 1;
    if ($expiresInFutureIn304) {
        print "Expires: " . HTTP::Date::time2str(time() + 100) . "\n";
        $hasExpiresHeader = 1;
    }
}

if ($query->http && $query->param("Range") =~ /bytes=(\d+)-(\d+)/) {

    if ($1 < 6 && $2 < 6) {
        print "Status: 206\n";
    } else {
        print "Status: 416\n";
    }

    $hasStatusCode = 1;
}

foreach (@names) {
    next if ($_ eq "uniqueId");
    next if ($_ eq "include-body");
    next if ($_ eq "Status" and $hasStatusCode);
    next if ($_ eq "Expires" and $hasExpiresHeader);
    print $_ . ": " . $query->param($_) . "\n";
}

# Cookie is used for "return-200-at-first-response" function.
my $cookie = $query->cookie(-name => $uniqueId, -value => ($requestedCount + 1));
print "Set-Cookie: " . $cookie . "\n";

print "\n";
print "test" if $includeBody;
