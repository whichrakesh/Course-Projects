all:    lex.cc parse.cc main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih
	g++   --std=c++0x lex.cc parse.cc main.cc;
	./a.out < testprogram.c > junk;
	#egrep 'nextToken|reduce' junk
	dot -Tps graph.gv -o graph.ps
        

lex.cc: lex.l Scanner.ih 
	./cond_remove_scannerih; 
	flexc++ lex.l; 
	sed -i '/include/a #include "Parserbase.h"' Scanner.ih; 

parse.cc: parse.y Parser.ih Parser.h Parserbase.h
	bisonc++  parse.y; 
#	bisonc++   --construction -V parse.y; 
#	sed -i '/ifndef/a #include "tree_util.hh"' Parserbase.h;
#	sed -i '/ifndef/a #include "tree.hh"' Parserbase.h;
#	./sedscript
     
clean: 
	rm Scanner* a.out lex.cc parse.cc junk graph* Parser.ih Parserbase.h

graph: all		
	xdg-open graph.ps

Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l
