
// There may be some flexer directives here

%%
[ \t\n]+                                    // skip white space chars. 

//keywords
"void"										return VOID;
"char"										return CHAR;
"int"										return INT;
"float"										return FLOAT;
"if" 										return IF;
"while"										return WHILE;							
"return" 									return RETURN;
"abstract"									return ABSTRACT;

//operators	
"||"										return OR_OP;
"&&"										return AND_OP;
"<="										return LE_OP;
">="										return GE_OP;
"=="										return EQ_OP;
"!="										return NE_OP;
"++"|"--"									return INC_OP;

['"(){}[\]=<>\*/,!;+-]									return matched()[0];


\/\*[^*]*(\*([^/*][^*]*)?)*\*\/							return COMMENT;
[[:alpha:]_][[:alpha:][:digit:]_]*          			return IDENTIFIER;

//constants

\'[^\\]\'|('\\[abfnrtv\\'"?]\')								return CHARACTER_CONSTANT;
([1-9][0-9]*)|(0[bB][01]*)|(0[0-7]*)|(0[xX][0-9a-fA-F]*) 	return INT_CONSTANT; 
[0-9]*\.[0-9]+(([eE][+-][0-9]+)|([eE][0-9]+))?				return FLOAT_CONSTANT;
\"[^\\"]*(\\.[^\\"]*)*\" 									return STRING_LITERAL;


.                                           				return matched()[0]; 
