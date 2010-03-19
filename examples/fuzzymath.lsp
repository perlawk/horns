#!/usr/bin/env horns

(print "1 + 1 = " (+ 1 1))

(set 'plus +)

(set '+ -)

(print "1 + 1 = " (+ 1 1))

(set '+ plus)

(print "1 + 1 = " (+ 1 1))