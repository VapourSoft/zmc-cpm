SOURCE = main.c panel.c operations.c vt100.c
HEADER = zmc.h
COMS = zmc.com zmc8080.com


# ZMC build is using:
#  The sccz80 assembler (defaults to __smallc linkage)
#  The classic library
#  The 'cpm' target personality


# the complete build is more compact (~ -500 byte) than the modular build
zmc.com: $(SOURCE) $(HEADER)
	zcc +cpm -O3 -vn -Wall \
	-pragma-output:noprotectmsdos \
	-pragma-output:noredir \
	-DAMALLOC -pragma-define:CRT_STACK_SIZE=1024 \
	$(SOURCE) -o $@


zmc8080.com: $(SOURCE) $(HEADER)
	zcc +cpm -O3 -vn -clib=8080 -Di8080 -Wall \
	-pragma-output:noprotectmsdos \
	-pragma-output:noredir \
	-DAMALLOC -pragma-define:CRT_STACK_SIZE=1024 \
	$(SOURCE) -o $@


.PHONY: all
all: $(COMS)


.PHONY: clean
clean:
	rm -f $(OBJS) $(COMS)
