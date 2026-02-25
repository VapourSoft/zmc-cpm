; test program to check function keys, exit wit ESC ESC

LF              equ     $0a
CR              equ     $0d
ESC             equ     $1b

                org $0100

                ld      b,0
loop:
                ld      hl,($0001)      ; hl -> BIOS fkt 1
                ld      de,6
                add     hl,de           ; hl -> BIOS fkt 3 CONIN
                ld      de,tstESC
                push    de              ; ret addr
                jp      (hl)            ; execute BIOS call

tstESC:                                 ; come back here
                cp      ESC             ; A == ESC?
                jr      nz,noESC        ; no
                dec     b               ; check for ESC ESC
                ret     z               ; yes, return to CP/M
                ld      b,1             ; no, but remember ESC seen
                jr      yesESC
noESC:
                ld      b,0             ; last char != ESC
yesESC:
                call    prbyte
                ld      a,$0d
                call    echo
                ld      a,$0A
                call    echo
                jr      loop

prbyte:
                ld      de,3
                add     hl,de           ; hl -> BIOS fkt 4 CONOUT
                push    af              ; Save A for LSD.
                srl     a
                srl     a
                srl     a               ; MSD to LSD position.
                srl     a
                call    prhex           ; Output hex digit.
                pop     af              ; Restore A.

prhex:
                and     $0F             ; Mask LSD for hex print.
                or      $30             ; Add "0".
                cp      $3A             ; Digit?
                jr      c,echo          ; Yes, output it.
                adc     $07             ; Add offset for letter.
echo:
                ld      c,a
                jp      (hl)            ; call BIOS

