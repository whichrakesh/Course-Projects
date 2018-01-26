all:    clean lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih node.h node.cc
	@./sedscript
	g++  -g --std=c++11 lex.cc parse.cc main.cc type_check.cc node.cc -o c-compiler;	


lex.cc: lex.l Scanner.ih
	flexc++ lex.l;
#	sed -i '/include/a #include "Parserbase.h"' Scanner.ih;

parse.cc: parse.y Parser.ih Parser.h Parserbase.h
	bisonc++  parse.y;
#	bisonc++   --construction -V parse.y;
#	sed -i '/ifndef/a #include "node.h"' Parserbase.h;
#	sed -i '/ifndef/a #include "tree.hh"' Parserbase.h;
#	./sedscript

clean:
	rm -f Scanner* c-compiler a.out junk lex.cc parse.cc code.asm graph* Parser.ih Parserbase.h symboltable.txt

graph: all
	xdg-open graph.ps

run: machine.cc
	./c-compiler  < testprogram.c > junk;
	g++ -g3 -O0 machine.cc
		
Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l
