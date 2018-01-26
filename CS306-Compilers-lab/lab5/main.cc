#include <iostream>
#include "Scanner.h"
#include "Parser.h"
using namespace std;

int main (int argc, char** arg)
{	
  	Parser parser;
  	if(argc > 1){
		for(int i=1; i<argc; i++){
			parser.functions_to_print.push_back(string(arg[i]));
		}
	}
  	parser.parse();
  	printCode();
}


