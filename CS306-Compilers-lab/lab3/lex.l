
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


[[:alpha:]_][[:alpha:][:digit:]_]*          			{Parser::d_val__ = std::string(matched()); return Parser::IDENTIFIER;}

//constants

//\'[^\\]\'|('\\[abfnrtv\\'"?]\')							return Parser::CHARACTER_CONST;
([1-9][0-9]*)|(0[bB][01]*)|(0[0-7]*)|(0[xX][0-9a-fA-F]*) 	{Parser::d_val__ = atoi(matched().c_str()); return Parser::INT_CONSTANT;} 
[0-9]*\.[0-9]+(([eE][+-][0-9]+)|([eE][0-9]+))?				{Parser::d_val__ = (float) atof(matched().c_str()); return Parser::FLOAT_CONSTANT;}
\"[^\\"]*(\\.[^\\"]*)*\" 									{Parser::d_val__ = std::string(matched()); return Parser::STRING_LITERAL;}


.                                           				return matched()[0]; 
