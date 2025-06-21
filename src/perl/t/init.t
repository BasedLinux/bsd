use strict;
use warnings;
use Test2::V0;

use Bsd::Store;

my $s = new Bsd::Store("dummy://");

my $res = $s->isValidPath("/bsd/store/g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-bar");

ok(!$res, "should not have path");

done_testing;
