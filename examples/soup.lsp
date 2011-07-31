#!/usr/bin/env horns

(set 'a 1)
(set 'b 2)
(set 'c 3)

(print 'a)
(print 'b)
(print 'c)

(set 'd (+ a c))

(print d)

(set 'e (+ b c))

(print e)

(set 'f (+ a (+ b c)))

(print f)

(+ 1 z)

; error: id z is not defined (line 24)
; error: + used improperly. check reference manual (line 24)