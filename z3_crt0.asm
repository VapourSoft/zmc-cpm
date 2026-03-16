;
;       Startup for CP/M
;
;       Stefano Bodrato - Apr. 2000
;                         Apr. 2001: Added MS-DOS protection
;
;	Dominic Morris  - Jan. 2001: Added argc/argv support
;			- Jan. 2001: Added in malloc routines
;			- Jan. 2001: File support added
;
;       $Id: cpm_crt0.asm $
;
; 	There are a couple of #pragma commands which affect this file:
;
;	#pragma output noprotectmsdos - strip the MS-DOS protection header
;	#pragma output protect8080 - add a check to block the program when on an 8080 CPU (not compatible)
;


    MODULE  cpm_crt0

    defc    crt0 = 1
    INCLUDE "zcc_opt.def"



    EXTERN	cpm_platform_init
    EXTERN	__bdos
    EXTERN    _main		;main() is always external to crt0

    PUBLIC    __Exit		;jumped to by exit()
    PUBLIC    l_dcal		;jp(hl)

    PUBLIC    __cpm_base_address

    defc    TAR__clib_exit_stack_size = 32
    ; Set sp to be &bdos, this sorts out CP/M 3 compatibility
    defc    TAR__register_sp = -6
    defc	__CPU_CLOCK = 4000000

    IF !DEFINED_CRT_ORG_CODE
        defc    CRT_ORG_CODE  = $100
    ENDIF

    IF !DEFINED_CPM_BASE_ADDRESS
        defc    __cpm_base_address = CRT_ORG_CODE - $100
	ELSE
        defc    __cpm_base_address = CPM_BASE_ADDRESS
	ENDIF

    IF !DEFINED_CLIB_OPEN_MAX
        defc    DEFINED_CLIB_OPEN_MAX = 1
        defc    CLIB_OPEN_MAX = 3
    ENDIF

    ; Default to some "sensible" values
    IF !DEFINED_CONSOLE_ROWS
        defc    CONSOLE_ROWS = 24
    ENDIF
    IF !DEFINED_CONSOLE_COLUMNS
        defc    CONSOLE_COLUMNS = 80
    ENDIF

    ; Default CLS to the ADM-3a code
    PUBLIC  CLIB_FPUTC_CLS_CODE
    IF !DEFINED_CLIB_FPUTC_CLS_CODE
        defc    CLIB_FPUTC_CLS_CODE = 0x1a
    ENDIF

    ; fputc_cons can use vt100 codes as well
    PUBLIC  CLIB_CPM_NATIVE_VT100
    IF !DEFINED_CLIB_CPM_NATIVE_VT100
        defc    CLIB_CPM_NATIVE_VT100 = 0
    ENDIF

    INCLUDE "crt/classic/crt_rules.inc"

    PUBLIC Z3ENV, ENVPTR

    org     CRT_ORG_CODE

;----------------------
; Execution starts here
;----------------------

    jp      start
Z3ENV:
    defm    "Z3ENV"
    defb    1
ENVPTR:
    defb    0,0
start:
    ld	a,$7F		; 01111111 into accumulator
    inc	a		; make it overflow ie. 10000000
    jp	pe,isz80	; only 8080 resets for odd parity here

    ld	c,9		; print string
    ld	de,err8080
    call	__bdos	; BDOS
    jp	0
err8080:
    defm	"This program requires a Z80 CPU."
    defb	13,10,'$'
isz80:

    ld      hl,0
    add     hl,sp
    ld      (__restore_sp_onexit+1),hl	;Save entry stack
    INCLUDE "crt/classic/crt_init_sp.inc"
    call    crt0_init
    INCLUDE "crt/classic/crt_init_atexit.inc"
    call    cpm_platform_init	;Any platform specific init

    INCLUDE "crt/classic/crt_init_heap.inc"
    INCLUDE "crt/classic/crt_init_eidi.inc"

IF CRT_ENABLE_COMMANDLINE = 1
    ld      hl,__cpm_base_address+$80
    ld      a,(hl)
    ;ld      b,0
    ld      b,h
    and     a
    jp      z,argv_done
    ;inc	hl
    ld      c,a
    add     hl,bc   ;now points to the end of the command line
    inc     hl
    ld      (hl),0
    dec     hl
    dec     c
    INCLUDE	"crt/classic/crt_command_line.inc"
    push    hl	;argv
    push    bc	;argc
ELSE
    ld      hl,0
    push    hl  ;argv
    push    hl  ;argc
ENDIF
    call    _main		;Call user code
    pop     bc	;kill argv
    pop     bc	;kill argc

__Exit:
    ld      (__cpm_base_address+$80),hl   ;Save exit value for CP/M 2.2
    push    hl		;Save return value
    call    crt0_exit
    pop     hl
    INCLUDE "crt/classic/crt_exit_eidi.inc"

; For CP/M 3 return the exit value via BDOS P_CODE
PUBLIC __restore_sp_onexit
__restore_sp_onexit:
    ld      sp,0	;Pick up entry sp
    ld      c,12        ;Get CPM version
    call    __bdos
    cp      $30
    jp      c,__cpm_base_address    ;Warm boot for CP/M < 3
    ld      a,l
    and     127
    ld      e,a
    ld      a,h         ;Exit with d=$ff for error, or d=$00 for no error
    or      l
    jr      z,do_exit_v3
    ld      a,255       ;Indicate error
do_exit_v3:
    ld      d,a
    ld      c,108
    call    __bdos           ;Report error
    jp      __cpm_base_address

l_dcal:	jp	(hl)		;Used for call by function ptr

    INCLUDE "crt/classic/crt_runtime_selection.inc"
    INCLUDE "crt/classic/crt_section.inc"
    INCLUDE "crt/classic/crt_cpm_fcntl.inc"


    SECTION code_crt_init
    ld      c,25
    call    __bdos
    ld      (defltdsk),a


    SECTION code_crt_exit
    ld      a,(defltdsk)        ;Restore default disc
    ld      e,a
    ld      c,14
    call    __bdos

