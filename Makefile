SOURCE = main.c panel.c operations.c vt100.c
HEADER = zmc.h
COMS = zmc.com zmc8080.com
OBJS = main.o panel.o operations.o vt100.o


zmc.com: $(OBJS)
	zcc +cpm -O3 $(OBJS) \
	-pragma-output:noprotectmsdos \
	-pragma-output:noredir \
	-pragma-define:CRT_STACK_SIZE=1024 \
	-o $@


main.o: main.c zmc.h
	zcc +cpm -O3 -vn -DAMALLOC -Wall -c $<

panel.o: panel.c zmc.h
	zcc +cpm -O3 -vn -Wall -c $<

operations.o: operations.c zmc.h
	zcc +cpm -O3 -vn -Wall -c $<

vt100.o: vt100.c zmc.h
	zcc +cpm -O3 -vn -Wall -c $<


#zmc.com: $(SOURCE) $(HEADER)
#	zcc +cpm -O3 -vn -DAMALLOC -pragma-define:CRT_STACK_SIZE=1024 -Wall \
#	$(SOURCE) -o $@


zmc8080.com: $(SOURCE) $(HEADER)
	zcc +cpm -clib=8080 -O3 -vn -Di8080 -DAMALLOC -Wall \
	-pragma-output:noprotectmsdos \
	-pragma-output:noredir \
	-pragma-define:CRT_STACK_SIZE=1024 \
	$(SOURCE) -o $@


.PHONY: all
all: $(COMS)


.PHONY: clean
clean:
	rm -f $(OBJS) $(COMS)
