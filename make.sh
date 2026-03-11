#!/bin/bash
# Compile z88dk for Z80 / CP/M
# -O3: maximal optimisation
# -vn: no verbosity
# -create-app: Build a .COM file

zcc +cpm -O3 -v -Wall \
-DAMALLOC -pragma-define:CRT_STACK_SIZE=1024 \
-pragma-output:noprotectmsdos \
-pragma-output:noredir \
main.c panel.c operations.c vt100.c -o zmc.com -create-app

if [ $? -eq 0 ]; then
    echo "✅ Build OK: ZMC.COM generated."
    # If you use an emulator, you can transfer it to the *.dsk
    # cpmcp -f connor z80.dsk ZMC.COM 0:
else
    echo "❌ Build error."
fi
