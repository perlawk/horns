#!/usr/bin/env python

import time

start=time.time()

i=0

while i<1e7: i+=1

end=time.time()

print "Speedy ran in %d seconds." % (end - start)

# Benchmarks:
#
# MacBook Pro 64bit 2.53GHz 4GB 1067 MHz DDR3 / Mac OS X 10.6.2
# Speedy ran in 1 seconds.