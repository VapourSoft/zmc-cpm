#ifndef __VLIB_H
#define __VLIB_H

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void
z3vinit(char *z3env) __z88dk_fastcall;

extern int
stndout(void) __z88dk_callee;

extern int
stndend(void) __z88dk_callee;

extern int
gotoxy(int xy) __z88dk_fastcall;

extern int
cls(void) __z88dk_callee;

extern int
clreos(void) __z88dk_callee;

extern int
ereol(void) __z88dk_callee;

extern void
gxymsg(char *str) __z88dk_fastcall;

extern void
vprint(char *str) __z88dk_fastcall;

extern void
vpstr(char *str) __z88dk_fastcall;

extern void
dellin(void) __z88dk_callee;

extern void
inslin(void) __z88dk_callee;

extern void
curon(void) __z88dk_callee;

extern void
curoff(void) __z88dk_callee;

#endif
