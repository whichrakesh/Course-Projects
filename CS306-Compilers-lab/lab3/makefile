all:    clean lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih node.h node.cc
	./sedscript
	g++   --std=c++11 lex.cc parse.cc main.cc node.cc;
	./a.out < testprogram.c > junk;
	#egrep 'nextToken|reduce' junk
	#dot -Tps graph.gv -o graph.ps


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
	rm -f Scanner* a.out lex.cc parse.cc junk graph* Parser.ih Parserbase.h

graph: all
	xdg-open graph.ps

Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l
