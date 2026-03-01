COMS = zmc.com zmc8080.com
OBJS = main.o panel.o operations.o globals.o vt100.o


zmc.com: $(OBJS) Makefile
	zcc +cpm -O3 $(OBJS) -pragma-define:CRT_STACK_SIZE=1024 -create-app -o $@


main.o: main.c zmc.h Makefile
	zcc +cpm -O3 -vn -DAMALLOC -Wall -c $<

panel.o: panel.c zmc.h
	zcc +cpm -O3 -vn -Wall -c $<

operations.o: operations.c zmc.h
	zcc +cpm -O3 -vn -Wall -c $<

globals.o: globals.c zmc.h
	zcc +cpm -O3 -vn -Wall -c $<

vt100.o: vt100.c zmc.h
	zcc +cpm -O3 -vn -Wall -c $<


#zmc.com: main.c panel.c operations.c globals.c vt100.c zmc.h
#	zcc +cpm -O3 -vn -DAMALLOC -pragma-define:CRT_STACK_SIZE=1024 -Wall \
#	main.c panel.c operations.c globals.c vt100.c -o zmc.com


zmc8080.com: main.c panel.c operations.c globals.c vt100.c zmc.h
	zcc +cpm -clib=8080 -O3 -vn -Di8080 -DAMALLOC -pragma-define:CRT_STACK_SIZE=1024 -Wall \
	main.c panel.c operations.c globals.c vt100.c -o zmc8080.com -create-app


.PHONY: all
all: $(COMS)


.PHONY: clean
clean:
	rm -f $(OBJS) $(COMS)
