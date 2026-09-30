; Poll the Amstrad CP/M Plus keyboard manager for one physical key event.
; Returns HL=$FFFF when no key is available; otherwise H=shift bitmap,
; L=PCW key number. Only use this on Amstrad CP/M Plus systems.

SECTION code_user

PUBLIC _pcw_get_key_nowait
PUBLIC _pcw_put_key
EXTERN subuserf

_pcw_get_key_nowait:
    call subuserf
    defw $00DA             ; KM KT GET
    jr nc, no_key
    ld h, b
    ld l, c
    ret

no_key:
    ld hl, $FFFF
    ret

; Reinsert a physical key event so CP/M CONIN can apply the active key map.
; Fastcall uint16_t in HL: H=shift bitmap, L=PCW key number.
_pcw_put_key:
    ld b, h
    ld c, l
    call subuserf
    defw $00DD             ; KM KT PUT
    ret