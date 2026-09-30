SOURCE = main.c panel.c operations.c pcw_platform.c cpsrcdst.asm keys.c pcwkeys.asm pcw_env.asm vlib.asm
ASM_INCLUDE_DEPS = pcw.asm pcw_sysenv.asm
ENVSRC = sysenv.asm vt100.asm
CRT = z3_crt0.asm
HEADER = zmc.h pcw_platform.h internal_env.h vlib.h
ZMC = zmc.com
TCAP = vt100.z3t vt100_ul.z3t adm-3a.z3t heath19.z3t
ENV = vt100.env vt100_ul.env adm-3a.env heath19.env

ZCC := $(shell command -v zcc 2>/dev/null)
ifndef ZCC
ZCC := docker run --rm -v $(CURDIR):/src -w /src z88dk/z88dk zcc
endif

.DEFAULT_GOAL := all

# Project/toolchain configuration shared by release and DeZog builds.
ZCC_TARGET = +cpm
ZCC_FLAGS = -vn -Wall \
	-pragma-output:noprotectmsdos \
	-pragma-output:noredir \
	-DAMALLOC -pragma-define:CRT_STACK_SIZE=512
ZCC_LIBS = -lvlib -lsyslib

# Include the DEZOG build
DEZOG_TARGET ?= $(ZMC)
include Makefile.dezog.mk

.PHONY: all
all: $(ZMC)

.PHONY: terminal-assets
terminal-assets: $(TCAP) $(ENV)


# ZMC build is using:
#  The sccz80 assembler (defaults to __smallc linkage)
#  The classic library
#  The 'cpm' target personality


# the complete build is more compact (~ -500 byte) than the modular build
$(ZMC): $(SOURCE) $(HEADER) $(ASM_INCLUDE_DEPS) $(CRT)
	$(ZCC) $(ZCC_TARGET) -O3 -crt0=$(CRT) $(ZCC_FLAGS) \
	$(SOURCE) $(ZCC_LIBS) \
	-o $@ -m --list

# the TCAP files
adm-3a.z3t: adm-3a.asm
	$(ZCC) +z80 -O3 --no-crt -vn -Wall $< -o $@ -m --list

heath19.z3t: heath19.asm
	$(ZCC) +z80 -O3 --no-crt -vn -Wall $< -o $@ -m --list

vt100.z3t: vt100.asm
	$(ZCC) +z80 -O3 --no-crt -vn -Wall $< -o $@ -m --list

vt100_ul.z3t: vt100_ul.asm
	$(ZCC) +z80 -O3 --no-crt -vn -Wall $< -o $@ -m --list


# the environment file
sysenv.bin: sysenv.asm
	$(ZCC) +z80 -O3 --no-crt -vn -Wall $< -o $@ -m --list

# the environment and TCAP files
adm-3a.env: sysenv.bin adm-3a.z3t
	cat $^ > $@

heath19.env: sysenv.bin heath19.z3t
	cat $^ > $@

vt100.env: sysenv.bin vt100.z3t
	cat $^ > $@

vt100_ul.env: sysenv.bin vt100_ul.z3t
	cat $^ > $@

.PHONY: clean
clean:
	rm -f $(ZMC) $(ENV) sysenv.bin *.map *.lis
