#include <iostream>
#include "Scanner.h"
using namespace std;
int main()
{
  Scanner scanner;   // define a Scanner object
  while (int token = scanner.lex()) // get all tokens
    {
      string const &text = scanner.matched();
      switch (token)
	{
	case Scanner::IDENTIFIER:
	  cout << "identifier: " << text << '\n';
	  break;

	case Scanner::FLOAT_CONSTANT:
	case Scanner::CHARACTER_CONSTANT:
	case Scanner::INT_CONSTANT:
	case Scanner::STRING_LITERAL:
	  cout << "constant: " << text << '\n';
	  break;
	    
	case Scanner::COMMENT:
	  cout << "comment: " << text << '\n';
	  break;  
	case Scanner::VOID:
		cout<<"type:"<<text<<'\n';
		break;
	case Scanner::CHAR:
		cout<<"type:"<<text<<'\n';
		break;
	case Scanner::INT:
		cout<<"type:"<<text<<'\n';
		break;
	case Scanner::FLOAT:
		 cout << "type: " << text << '\n';
	  	break ; 
	case Scanner::IF:
		cout<<"keyword:"<<text<<'\n';
		break;
	case Scanner::WHILE:
		cout<<"keyword"<<text<<'\n';
		break;
	case Scanner::RETURN:
		  cout << "keyword: " << text << '\n';
		  break ; 
	case Scanner::OR_OP:
		cout<<"OR_OP"<<text<<'\n';
		break;
	case Scanner::AND_OP:
	case Scanner::LE_OP:
	case Scanner::GE_OP:
	case Scanner::EQ_OP:
	case Scanner::NE_OP:    
	case Scanner::INC_OP:
	  cout << "operation: " << text << '\n';
	  break ;  
	case '\'' :
	case '\"' :
	case '('  :
	case ')'  :
	case '['  :
	case ']'  :
	case '{'  :
	case '}'  : 
	case ';'  :
	case '='  :
	case '<'  :
	case '>'  :
	case '*'  :
	case '/'  :
	case ','  :
	case '!'  :
	case '+'  :
	case '-'  :
		cout << "character token:'" << text <<"'\n";
		break;  	
	default:
	  cout << "-------------------Not a token: '" << text << "'\n";
	}
    }
}
