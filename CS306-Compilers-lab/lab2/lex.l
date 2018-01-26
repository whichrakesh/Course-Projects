
// There may be some flexer directives here

%%
[ \t\n]+                                    // skip white space chars. 

//keywords
"void"										return Parser::VOID;
"int"										return Parser::INT;
"float"										return Parser::FLOAT;
"if" 										return Parser::IF;
"else"										return Parser::ELSE;
"while"										return Parser::WHILE;	
"for"										return Parser::FOR;
"return" 									return Parser::RETURN;

//operators	
"||"										return Parser::OR_OP;
"&&"										return Parser::AND_OP;

"<="										return Parser::LE_OP;
">="										return Parser::GE_OP;
"=="										return Parser::EQ_OP;
"!="										return Parser::NE_OP;
"++"|"--"									return Parser::INC_OP;

['"(){}[\]=<>\*/,!;+-]									return matched()[0];


\/\*[^*]*(\*([^/*][^*]*)?)*\*\/							std::cout << "comment";
[[:alpha:]_][[:alpha:][:digit:]_]*          			return Parser::IDENTIFIER;

//constants


([1-9][0-9]*)|(0[bB][01]*)|(0[0-7]*)|(0[xX][0-9a-fA-F]*) 	return Parser::INT_CONSTANT; 
[0-9]*\.[0-9]+(([eE][+-][0-9]+)|([eE][0-9]+))?				return Parser::FLOAT_CONSTANT;
\"[^\\"]*(\\.[^\\"]*)*\" 									return Parser::STRING_LITERAL;


.                                           				return matched()[0]; 
