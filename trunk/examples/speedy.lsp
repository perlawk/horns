#!/usr/bin/env horns

(set 'start (time))

(for (i 0 1e7) i)

(set 'end (time))

(print "Speedy ran in " (- end start) " seconds.")

; Benchmarks:
;
; MacBook Pro 64bit 2.53GHz 4GB 1067 MHz DDR3 / Mac OS X 10.6.2
; Speedy ran in 6 seconds.
;
; MacBook Pro 64bit 2.53GHz 4GB 1067 MHz DDR3 / Windows 7 64bit
; Speedy ran in 8 seconds.
;
; MacBook Pro 64bit 2.53GHz 4GB 1067 MHz DDR3 / Mac OS X 10.6.2 / VMWare Fusion 3.0.1 / Ubuntu 9.10
; Speedy ran in 22 seconds.
