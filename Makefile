CC = gcc
LD=ld
CFLAGS = -O2 -c
LFLAGS = --shared
LEX = flex
YACC = byacc
APX = apxs
APXFLAGS = -i -a -c

default: mod_defunc

apache13: mod_defunc13

mod_defunc13: y.o lex.o mod_defunc.1.3.c
	$(APX) -i -A -c -n defunc mod_defunc.1.3.c lex.o y.o
	
mod_defunc: y.o lex.o mod_defunc.c
	$(APX) -i -A -c -n defunc mod_defunc.c lex.o y.o 
	
y.o: defunc.y
	$(YACC) -d defunc.y
	$(CC) $(CFLAGS) y.tab.c -o y.o
	
lex.o: defunc.lex
	$(LEX) defunc.lex
	$(CC) $(CFLAGS) lex.yy.c -o lex.o
	
clean:
	rm *.o *.so *.la *.lo *.slo lex.yy.c y.tab.h y.tab.c
	
dist: apache13.diff
	tar --bzip2 -cvvf mod_defunc.tar.bz2 mod_defunc.c Makefile defunc.y defunc.lex apache13.diff README

mod_defunc.1.3.c:
	patch mod_defunc.c apache13.diff -o  mod_defunc.1.3.c
    
