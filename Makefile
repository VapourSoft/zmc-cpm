SOURCE = main.c panel.c operations.c cpsrcdst.asm keys.c internal_env.asm vlib.asm
ENVSRC = sysenv.asm vt100.asm
CRT = z3_crt0.asm
HEADER = zmc.h internal_env.h vlib.h
ZMC = zmc.com
TCAP = vt100.tcp vt100_ul.tcp adm-3a.tcp heath19.tcp
ENV = vt100.env vt100_ul.env adm-3a.env heath19.env


.PHONY: all
all: $(ZMC) $(TCAP) $(ENV)


# ZMC build is using:
#  The sccz80 assembler (defaults to __smallc linkage)
#  The classic library
#  The 'cpm' target personality


# the complete build is more compact (~ -500 byte) than the modular build
$(ZMC): $(SOURCE) $(HEADER) $(ENVSRC) $(CRT)
	zcc +cpm -O3 -crt0=z3_crt0.asm -vn -Wall \
	-pragma-output:noprotectmsdos \
	-pragma-output:noredir \
	-DAMALLOC -pragma-define:CRT_STACK_SIZE=512 \
	$(SOURCE) -lvlib -lsyslib \
	-o $@ -m --list

# the TCAP files
adm-3a.tcp: adm-3a.asm
	zcc +z80 -O3 --no-crt -vn -Wall $< -o $@ -m --list

heath19.tcp: heath19.asm
	zcc +z80 -O3 --no-crt -vn -Wall $< -o $@ -m --list

vt100.tcp: vt100.asm
	zcc +z80 -O3 --no-crt -vn -Wall $< -o $@ -m --list

vt100_ul.tcp: vt100_ul.asm
	zcc +z80 -O3 --no-crt -vn -Wall $< -o $@ -m --list


# the environment file
sysenv.bin: sysenv.asm
	zcc +z80 -O3 --no-crt -vn -Wall $< -o $@ -m --list

# the environment and TCAP files
adm-3a.env: sysenv.bin adm-3a.tcp
	cat $^ > $@

heath19.env: sysenv.bin heath19.tcp
	cat $^ > $@

vt100.env: sysenv.bin vt100.tcp
	cat $^ > $@

vt100_ul.env: sysenv.bin vt100_ul.tcp
	cat $^ > $@


.PHONY: clean
clean:
	rm -f $(ZMC) $(ENV) $(TCP) *.map *.lis
