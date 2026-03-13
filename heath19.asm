; Z3TCAP:  heath19.asm
; Author:  Harold F. Bower
; Date  :  11 Oct 1992

; Z3 Termcap implementing the Heath/Zenith-19 command set

ESC     EQU     27              ; Escape character

; New Terminal Capabilities Data

Z3TCAP: DEFB    "Heath/Z-19   " ; Name of terminal (13 chars,   |
                                ;       Space-terminated.       |
B13:    DEFB    GOELD-Z3TCAP    ; Offset to DOELD in Graphics   |
                                ;       section.                |
B14:    DEFB    10000000B       ; Bit 7 = Extended TCAP, rest   |
                                ;       undefined.              |
B15:    DEFB    00000001B       ; Rev Vid, Wrap, Scroll, ASCII  |
                                ; B4 - 0 = ASCII, 1 = ANSI      |
                                ; B3 - 0 = Scroll @ EOP,        |
                                ;      1 = No Scroll            |

                                ; B2 - 0 = Line Wrap            |
                                ;      1 = No Wrap on write to  |
                                ;           last char in line   |
                                ; B1 - 0 = No Delay, 1 = 10 Sec |
                                ; B0 - 0 = Half-Intensity avail |
                                ;      1 - Rev Video available  |
                        ; Single-char Cursor Motion
        DEFB    'E'-'@'         ; Cursor up  (WS Diamond)
        DEFB    'X'-'@'         ; Cursor down
        DEFB    'D'-'@'         ; Cursor right
        DEFB    'S'-'@'         ; Cursor left
                        ; Delay values for Functions
        DEFB    3               ; CL delay
        DEFB    0               ; CM delay
        DEFB    0               ; CE delay
                        ; Strings start here.
        DEFB    ESC,'E',0       ; CL str (Clear, Home Cursor)
        DEFB    ESC,"Y%+ %+ ",0 ; CM str (Cursor positioning)
        DEFB    ESC,'K',0       ; CE str (Clear to End-of-Line)

        DEFB    ESC,'p',0       ; SO str (Go to Reverse Video)
        DEFB    ESC,'q',0       ; SE str (Return Normal Video)
        DEFB    0               ; TI str (Initialize Terminal)
        DEFB    0               ; TE str (De-initialize Term)
                        ; Extensions to Standard TCAP
        DEFB    ESC,'M',0       ; LD str (Delete Line)
        DEFB    ESC,'L',0       ; LI str (Insert Line)
        DEFB    ESC,'J',0       ; CD - Clear to EOS String
                        ; Attribute-setting parameters
        DEFB    0               ; SA - Set Attrs Comnd string
        DEFB    0               ; AT - Attribute String
                        ; Read data from terminal
        DEFB    ESC,'n',0       ; RC - Report Corsor Pos'n as:
                                ;       ESC Y Pr Pc
        DEFB    0               ; RL - Read line until cursor

GOELD:  DEFB    0               ; GO/GE - Graphics On/Off Delay
                        ; Graphics strings (offset from Delay)
        DEFB    ESC,'F',0       ; GO - Graphics Mode On
        DEFB    ESC,'G',0       ; GE - Graphics Mode End
        DEFB    ESC,"x5",0      ; CDO - Cursor Off string
        DEFB    ESC,"y5",0      ; CDE - Cursor Enable string
                        ; Graphics Characters
        DEFB    'f'             ; GULC - Upper Left Corner  [*]
        DEFB    'c'             ; GURC - Upper Right Corner [*]
        DEFB    'e'             ; GLLC - Lower Left Corner  [*]
        DEFB    'd'             ; GLRC - Lower Right Corner [*]
        DEFB    'a'             ; GHL - Horizontal Line     [-]
        DEFB    '`'             ; GVL - Vertical Line       [|]
        DEFB    'i'             ; GFB - Full Block String   [*]
        DEFB    'w'             ; GHB - Hashed Block String [#]
        DEFB    'u'             ; GUI - Upper Intersection  [+]
        DEFB    's'             ; GLI - Lower Intersection  [+]
        DEFB    'b'             ; GIS - Intersection        [+]
        DEFB    'v'             ; GRTI - Right Intersection [+]
        DEFB    't'             ; GLTI - Left Intersection  [+]

;------------- End of Sample TermCap -------------
;------------- End of Sample TermCap -------------

ALIGN   128
