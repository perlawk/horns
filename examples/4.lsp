#!/usr/bin/env horns

(print (+ 2 2))

;         Parse Tree:
;
;           seq
;         /     \
;        exp    seq
;         |      |
;        list   tail
;      /  |   \
;    (   seq   )
;      /     \
;    exp       seq
;     |       /    \
;   atom    exp      seq
;     |      |     /      \
;  symbol   atom  exp       seq
;     |      |     |       /   \
;   print  symbol atom   exp  tail
;            |     |      |
;            +   number atom
;                  |      |
;                  2    number
;                         |
;                         2

(print (- 6 2))

(print (* 2 2))

(print (/ 8 2))
