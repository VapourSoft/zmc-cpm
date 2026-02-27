```
======================================================================
         Z80 MANAGEMENT COMMANDER (ZMC) - Version 1.2
           "The Crystallized & Global Release"
              by Volney Torres & Martin H.R.
======================================================================
```

## DESCRIPTION
ZMC is a dual-panel file manager inspired by Norton Commander,
specifically designed for CP/M systems based on Z80 processors,
but an 8080 version is also available.
Version 1.2 consolidates the code structure for superior speed and
global compatibility.

## WHAT'S NEW IN V1.2 (The Ho-Ro Update)
- CP/M 3 SUPPORT: Native display of file date and time stamps.
- ALPHABETIC SORTING: Fast QSort implementation for quick navigation.
- ATTRIBUTE MANAGEMENT: View Read-Only, System, and Archive flags.
- OPTIMIZED ENGINE: Drastically improved file size calculation.
- MODULAR CORE: Separated into globals.c and operations.c for stability.
- TECHNICAL TRANSLATION: Standardized English UI and code comments.

## KEYMAP

| KEY | Function |
|
| `[Arrows Up/Down]`   | Navigate the file list.
| `[TAB]`              | Switch active panel (A <-> B).
| `[Space]`            | Tag file for batch operations (*).
| `[F1]`  or `[ESC]1`  | Quick HELP and version credits.
| `[F3]`  or `[ESC]3`  | VIEW file with MORE/ESC support.
| `[F4]`  or `[ESC]4`  | DUMP file with MORE/ESC support.
| `[F5]`  or `[ESC]5`  | Batch COPY.
| `[F8]`  or `[ESC]8`  | Batch DELETE.
| `[F10]` or `[ESC]0`  | EXIT to system prompt.
| `[ESC][ESC]`         | EXIT to system prompt.

The key handling suports the standard `VT100` cursor and function keys as well as
the wordstar key bindings:

`^E` = UP, `^X` = DOWN, `^R` = PAGEUP, `^C` = PAGEDOWN

ZMC allows also to enter commands in the prompt line `A> ` followed by `[CR]`,
i.e.

- HELP
- VIEW (or TYPE or CAT)
- DUMP (or HEX)
- COPY (or CP)
- DEL (or ERA or RM),
- EXIT (or QUIT).

## TECHNICAL SPECIFICATIONS
- Compiler: z88dk (ZCC) with -O3 optimization.
- Z80 version `ZMC.COM` and 8080 version `ZMC8080.COM` available.
- Terminal: ANSI/VT100 (Full support for real hardware and emulators).
- Memory: Dynamic Heap management to support large directories.
- CP/M3: Program adapts the screen size automatically from SCB info.

### Development and configuration support

Calling the program with command line argument as `ZMC --CONFIG` shows system
information and the value and file position of the screen size constants:

```
CP/M version: 22
COLUMNS @ 0x4F44: 80
LINES @ 0x4F45: 24
MAX_FILES: 924
```
The byte values at `0x4F44` and `0x4F45` can be adjusted to your requirements with a hex editor.

Calling as `ZMC --KEY` supports the function key handling, the program simply shows
each received key byte as hex and ASCII, e.g. with ANSI/VT100 emulation, pressing
`F1 F2 F5 F6 ESC ESC` displays:

```
CP/M version 22: function key test - exit with <ESC><ESC>
0x1B  ESC
0x4F  O
0x50  P
0x1B  ESC
0x4F  O
0x51  Q
0x1B  ESC
0x5B  [
0x31  1
0x35  5
0x7E  ~
0x1B  ESC
0x5B  [
0x31  1
0x37  7
0x7E  ~
0x1B  ESC
0x1B  ESC
```

## INSPIRATION & CREDITS
ZMC is a tribute to the legendary Norton Commander and Peter Norton.

CONTRIBUTORS:
- Volney Torres (lu1pvt): Original creator, UI design, and panel logic.
- Martin Homuth-Rosemann (Ho-Ro): Global refactoring, CP/M Plus support,
  and core algorithm optimization.

(c) 2025-2026 - Open Project for the CP/M Community and VCFed.
