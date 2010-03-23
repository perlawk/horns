#!/usr/bin/env horns

(set 'c 1)

(set 'times *)
; comment the above line and the following error messages will appear:
; unknown function: times (line 9)
; error: + used improperly. check reference manual. (line 9)

(set 'f (lambda (x) (+ (times x x) c)))

(print "5 * 5 + 1 = " (f 5))