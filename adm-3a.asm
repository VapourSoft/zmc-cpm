; NZTCAP:  adm-3a.asm
; New Z3 Termcap for the Lear Siegler ADM-3A
;
;
ESC	EQU	27		; Escape character
;
; The first character in the terminal name must not be a space.  For
; Z3TCAP.TCP library purposes only, the name terminates with a space
; and must be unique in the first eight characters.
;
TNAME:	DB	"ADM-3A       "	; Name of terminal (13 chars)
;
GOFF:	DB	0		; Graphics offset from Z3TCAP start
;
; Terminal configuration bytes B14 and B15 are defined and bits assigned
; as follows.  The remaining bits are not currently assigned.  Set these
; bits according to your terminal configuration.
;
;	B14 b7: Z3TCAP Type.... 0 = Standard TCAP  1 = Extended TCAP
;
;	bit:	76543210
B14:	DB	00000000B	; Configuration byte B14
;
;	B15 b0: Standout....... 0 = Half-Intensity 1 = Reverse Video
;	B15 b1: Power Up Delay. 0 = None           1 = Ten-second delay
;	B15 b2: No Auto Wrap... 0 = Auto Wrap      1 = No Auto Wrap
;	B15 b3: No Auto Scroll. 0 = Auto Scroll    1 = No Auto Scroll
;	B15 b4: ANSI........... 0 = ASCII          1 = ANSI
;
;	bit:	76543210
B15:	DB	00000101B	; Configuration byte B15
;
; Single character arrow keys or WordStar diamond
;
	DB	'E'-40H		; Cursor up
	DB	'X'-40H		; Cursor down
	DB	'D'-40H		; Cursor right
	DB	'S'-40H		; Cursor left
;
; Delays (in ms) after sending terminal control strings
;
	DB	0		; CL delay
	DB	0		; CM delay
	DB	0		; CE delay

	; Terminal Capabilities Data
	;
	DB 'Z'-'@',0		;CL String
	DB ESC,"=%+ %+ ",0 	;CM String
	DB 'X'-'@',0		;CE String
	DB ESC,"B1",0 		;SO String
	DB ESC,"C1",0 		;SE String
	DB 0 			;TI String
	DB 0 			;TE String

;	END
;
; End of Z3TCAP
;

ALIGN 128

