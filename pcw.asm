; Z3TCAP terminal profile for the native Amstrad PCW CP/M CRT.
; The PCW terminal emulator uses PCW/Z19-like escapes, not ANSI/VT100.

ESC     EQU     27

TNAME:  DB      "AMSTRAD-PCW  " ; 13 characters, space padded
GOFF:   DB      0               ; No graphics section
B14:    DB      00000000B       ; Standard (non-extended) TCAP
B15:    DB      00000001B       ; Reverse video; non-ANSI terminal

        DB      0,0,0,0         ; No single-byte arrow key translations
        DB      0,0,0           ; No function delays

; Clear viewport, then home. PCW ESC E clears without homing.
        DB      ESC,'E',ESC,'H',0
; Cursor motion: ESC Y, row+32, column+32 (VLIB supplies zero-based coords).
        DB      ESC,"Y%+ %+ ",0
        DB      ESC,'K',0       ; Clear to end of line
        DB      ESC,'p',0       ; Reverse video on
        DB      ESC,'q',0       ; Reverse video off
        DB      0               ; Terminal initialization
        DB      0               ; Terminal deinitialization

        ALIGN   128