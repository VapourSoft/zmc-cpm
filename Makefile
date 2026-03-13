SOURCE = main.c panel.c operations.c keys.c internal_env.asm vlib.asm
ENVSRC = sysenv_vt100.asm
HEADER = zmc.h internal_env.h vlib.h
ZMC = zmc.com
ENV = vt100.env vt100_ul.env adm-3a.env heath19.env


.PHONY: all
all: $(ZMC) $(ENV)

.PHONY: test
test: $(TEST)

# ZMC build is using:
#  The sccz80 assembler (defaults to __smallc linkage)
#  The classic library
#  The 'cpm' target personality


# the complete build is more compact (~ -500 byte) than the modular build
$(ZMC): $(SOURCE) $(HEADER) $(ENVSRC)
	zcc +cpm -O3 -crt0=z3_crt0.asm -vn -Wall \
	-pragma-output:noprotectmsdos \
	-pragma-output:noredir \
	-DAMALLOC -pragma-define:CRT_STACK_SIZE=1024 \
	$(SOURCE) -lvlib -lsyslib \
	-o $@ -m --list


# the environment files
vt100.env: sysenv_vt100.asm
	z88run zcc +cpm -O3 --no-crt -vn -Wall $< -o $@ -m --list

# create intermediate source code
sysenv_vt100.asm: sysenv.asm vt100.asm
	cat $^ > $@

# some (untested) TCAPs
adm-3a.env: sysenv_adm-3a.asm
	z88run zcc +cpm -O3 --no-crt -vn -Wall $< -o $@ -m --list

sysenv_adm-3a.asm: sysenv.asm adm-3a.asm
	cat $^ > $@

heath19.env: sysenv_heath19.asm
	z88run zcc +cpm -O3 --no-crt -vn -Wall $< -o $@ -m --list

sysenv_heath19.asm: sysenv.asm heath19.asm
	cat $^ > $@

vt100_ul.env: sysenv_vt100_ul.asm
	z88run zcc +cpm -O3 --no-crt -vn -Wall $< -o $@ -m --list

sysenv_vt100_ul.asm: sysenv.asm vt100_ul.asm
	cat $^ > $@


.PHONY: clean
clean:
	rm -f $(ZMC) $(ENV) $(TEST) *.map *.lis sysenv_*
