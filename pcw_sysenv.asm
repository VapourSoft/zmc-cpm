; Internal ZCPR3 environment for the native 90x32 Amstrad PCW display.

ALIGN 128

PUBLIC _INTENV, _INTCOLUMNS, _INTLINES, _INTLINES2

_INTENV:
        JP      0000
        DB      "Z3ENV"
        DB      80H

        DW      0000
        DB      0
        DW      0000
        DB      0
        DW      0000
        DB      0
        DW      0000
        DB      0
        DW      0000
        DB      0
        DW      0000
        DB      0
        DW      0000
        DB      0
        DW      0000
        DB      0
        DB      0
        DW      0000
        DW      0000
        DW      0000
        DB      0
        DW      0000
        DB      0
        DB      'P'-'@'
        DB      31
        DB      1
        DB      0
        DB      0

_INTCOLUMNS:
        DB      90
_INTLINES:
        DB      32
_INTLINES2:
        DB      30

        DW      0000
        DB      00
        DB      80
        DB      66
        DB      58
        DB      1
        DB      00,00,00,00
        DW      0000
        DB      0
        DW      0000
        DB      0
        DW      0000
        DB      "SH      "
        DB      "VAR"
        DB      "        ","   "
        DB      "        ","   "
        DB      "        ","   "
        DB      "        ","   "

ALIGN 128
