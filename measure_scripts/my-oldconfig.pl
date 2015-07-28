#!/usr/bin/perl -w

use Expect;
my $exp = Expect->spawn("make oldconfig")
    or die "Cannot spawn make oldconfig: $!\n";

my $timeout = 10;
$exp->expect($timeout,
	     [ '^ *choice\[.*\]: '           => sub { $exp->send("\n");  exp_continue; } ],
	     [ '^([^ ]| *[^>0-9]).*\(NEW\) ' => sub { $exp->send("n\n"); exp_continue; } ],
	     [ "configuration written to .config" ],
    ) || die "Timeout"
