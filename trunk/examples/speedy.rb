#!/usr/bin/env ruby

require "time"

start_t=Time.now.to_i

0.upto(1e7) { |i| i }

end_t=Time.now.to_i

puts "Speedy ran in #{end_t - start_t} seconds."

# Benchmarks:
#
# MacBook Pro 64bit 2.53GHz 4GB 1067 MHz DDR3 / Mac OS X 10.6.2
# Speedy ran in 9 seconds.