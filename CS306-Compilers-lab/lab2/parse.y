%debug
%scanner Scanner.h
%scanner-token-function d_scanner.lex()

%token VOID INT FLOAT IF ELSE WHILE RETURN FOR
%token OR_OP AND_OP LE_OP GE_OP EQ_OP NE_OP INC_OP
%token IDENTIFIER  INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL
 

%%
translation_unit
  : function_definition 
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"translation_unit\"]\n";
		 create_edge($$,$1 );
	}
  | translation_unit function_definition
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"translation_unit\"]\n";
		 create_edge($$,$1 );
		 create_edge($$,$2 );
	}
  ;

function_definition
  : type_specifier fun_declarator compound_statement
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"function_definition\"]\n";
		 create_edge($$,$1 );
		 create_edge($$,$2 );
		 create_edge($$,$3 );
	}
  ;

type_specifier
  : VOID
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"type_specifier\"]\n";
		 fout << "N"<< count <<  " [label = \"VOID\"]\n";
		 create_edge($$,count);
		 count++;
	}
  | INT
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"type_specifier\"]\n";
		 fout << "N"<< count <<  " [label = \"INT\"]\n";
		 create_edge($$,count);
		 count++;
	}
  | FLOAT
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"type_specifier\"]\n";
		 fout << "N"<< count <<  " [label = \"FLOAT\"]\n";
		 create_edge($$,count);
		 count++;
	}
  ;

fun_declarator
  : IDENTIFIER '(' parameter_list ')'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"fun_declarator\"]\n";
		 fout << "N"<< count <<  " [label = \"IDENTIFIER\"]\n";
		 create_edge($$,count);
		 count++;
		 fout << "N"<< count <<  " [label = \"'('\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
		 fout << "N"<< count <<  " [label = \"')'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  | IDENTIFIER '(' ')'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"fun_declarator\"]\n";
		 fout << "N"<< count <<  " [label = \"IDENTIFIER\"]\n";
		 create_edge($$,count);
		 count++;
		 fout << "N"<< count <<  " [label = \"'('\"]\n";
		 create_edge($$,count);
		 count++;
		 fout << "N"<< count <<  " [label = \"')'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  ;

parameter_list
  : parameter_declaration
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"parameter_list\"]\n";
		 create_edge($$,$1 );
	}
  | parameter_list ',' parameter_declaration
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"parameter_list\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"','\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  ;

parameter_declaration
  : type_specifier declarator
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"parameter_declaration\"]\n";
		 create_edge($$,$1 );
		 create_edge($$,$2 );
	}
  ;

declarator
  : IDENTIFIER
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"declarator\"]\n";
		 fout << "N"<< count <<  " [label = \"IDENTIFIER\"]\n";
		 create_edge($$,count);
		 count++;
	}
  | declarator '[' constant_expression ']'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"declarator\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"'['\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
		 fout << "N"<< count <<  " [label = \"']'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  ;

constant_expression
        : INT_CONSTANT
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"constant_expression\"]\n";
		 fout << "N"<< count <<  " [label = \"INT_CONSTANT\"]\n";
		 create_edge($$,count);
		 count++;
	}
        | FLOAT_CONSTANT
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"constant_expression\"]\n";
		 fout << "N"<< count <<  " [label = \"FLOAT_CONSTANT\"]\n";
		 create_edge($$,count);
		 count++;
	}
        ;

compound_statement
  : '{' '}'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"compound_statement\"]\n";
		 fout << "N"<< count <<  " [label = \"'{'\"]\n";
		 create_edge($$,count);
		 count++;
		 fout << "N"<< count <<  " [label = \"'}'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  | '{' statement_list '}'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"compound_statement\"]\n";
		 fout << "N"<< count <<  " [label = \"'{'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$2 );
		 fout << "N"<< count <<  " [label = \"'}'\"]\n";
		 create_edge($$,count);
		 count++;
	}
        | '{' declaration_list statement_list '}'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"compound_statement\"]\n";
		 fout << "N"<< count <<  " [label = \"'{'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$2 );
		 create_edge($$,$3 );
		 fout << "N"<< count <<  " [label = \"'}'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  ;

statement_list
  : statement
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"statement_list\"]\n";
		 create_edge($$,$1 );
	}
  | statement_list statement
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"statement_list\"]\n";
		 create_edge($$,$1 );
		 create_edge($$,$2 );
	}
  ;

statement
        : compound_statement
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"statement\"]\n";
		 create_edge($$,$1 );
	}
  | selection_statement
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"statement\"]\n";
		 create_edge($$,$1 );
	}
  | iteration_statement
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"statement\"]\n";
		 create_edge($$,$1 );
	}
  | assignment_statement
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"statement\"]\n";
		 create_edge($$,$1 );
	}
        | RETURN expression ';'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"statement\"]\n";
		 fout << "N"<< count <<  " [label = \"RETURN\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$2 );
		 fout << "N"<< count <<  " [label = \"';'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  ;

assignment_statement
  : ';'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"assignment_statement\"]\n";
		 fout << "N"<< count <<  " [label = \"';'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  |  l_expression '=' expression ';'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"assignment_statement\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"'='\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
		 fout << "N"<< count <<  " [label = \"';'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  ;

expression
  : logical_and_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"expression\"]\n";
		 create_edge($$,$1 );
	}
  | expression OR_OP logical_and_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"OR_OP\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  ;

logical_and_expression
  : equality_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"logical_and_expression\"]\n";
		 create_edge($$,$1 );
	}
  | logical_and_expression AND_OP equality_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"logical_and_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"AND_OP\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  ;

equality_expression
  : relational_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"equality_expression\"]\n";
		 create_edge($$,$1 );
	}
  | equality_expression EQ_OP relational_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"equality_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"EQ_OP\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  | equality_expression NE_OP relational_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"equality_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"NE_OP\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  ;
relational_expression
  : additive_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"relational_expression\"]\n";
		 create_edge($$,$1 );
	}
  | relational_expression '<' additive_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"relational_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"'<'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  | relational_expression '>' additive_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"relational_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"'>'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  | relational_expression LE_OP additive_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"relational_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"LE_OP\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  | relational_expression GE_OP additive_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"relational_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"GE_OP\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  ;

additive_expression
  : multiplicative_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"additive_expression\"]\n";
		 create_edge($$,$1 );
	}
  | additive_expression '+' multiplicative_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"additive_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"'+'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  | additive_expression '-' multiplicative_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"additive_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"'-'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  ;

multiplicative_expression
  : unary_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"multiplicative_expression\"]\n";
		 create_edge($$,$1 );
	}
  | multiplicative_expression '*' unary_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"multiplicative_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"'*'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  | multiplicative_expression '/' unary_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"multiplicative_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"'/'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  ;
unary_expression
  : postfix_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"unary_expression\"]\n";
		 create_edge($$,$1 );
	}
  | unary_operator postfix_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"unary_expression\"]\n";
		 create_edge($$,$1 );
		 create_edge($$,$2 );
	}
  ;

postfix_expression
  : primary_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"postfix_expression\"]\n";
		 create_edge($$,$1 );
	}
  | IDENTIFIER '(' ')'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"postfix_expression\"]\n";
		 fout << "N"<< count <<  " [label = \"IDENTIFIER\"]\n";
		 create_edge($$,count);
		 count++;
		 fout << "N"<< count <<  " [label = \"'('\"]\n";
		 create_edge($$,count);
		 count++;
		 fout << "N"<< count <<  " [label = \"')'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  | IDENTIFIER '(' expression_list ')'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"postfix_expression\"]\n";
		 fout << "N"<< count <<  " [label = \"IDENTIFIER\"]\n";
		 create_edge($$,count);
		 count++;
		 fout << "N"<< count <<  " [label = \"'('\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
		 fout << "N"<< count <<  " [label = \"')'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  | l_expression INC_OP
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"postfix_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"INC_OP\"]\n";
		 create_edge($$,count);
		 count++;
	}
  ;

primary_expression
	: l_expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"primary_expression\"]\n";
		 create_edge($$,$1 );
	}
    |  l_expression '=' expression  
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"primary_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"'='\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
	| INT_CONSTANT
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"primary_expression\"]\n";
		 fout << "N"<< count <<  " [label = \"INT_CONSTANT\"]\n";
		 create_edge($$,count);
		 count++;
	}
	| FLOAT_CONSTANT
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"primary_expression\"]\n";
		 fout << "N"<< count <<  " [label = \"FLOAT_CONSTANT\"]\n";
		 create_edge($$,count);
		 count++;
	}
    | STRING_LITERAL
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"primary_expression\"]\n";
		 fout << "N"<< count <<  " [label = \"STRING_LITERAL\"]\n";
		 create_edge($$,count);
		 count++;
	}
	| '(' expression ')'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"primary_expression\"]\n";
		 fout << "N"<< count <<  " [label = \"'('\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$2 );
		 fout << "N"<< count <<  " [label = \"')'\"]\n";
		 create_edge($$,count);
		 count++;
	}
	;

l_expression
        : IDENTIFIER
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"l_expression\"]\n";
		 fout << "N"<< count <<  " [label = \"IDENTIFIER\"]\n";
		 create_edge($$,count);
		 count++;
	}
        | l_expression '[' expression ']' 
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"l_expression\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"'['\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
		 fout << "N"<< count <<  " [label = \"']'\"]\n";
		 create_edge($$,count);
		 count++;
	}
        ;
expression_list
        : expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"expression_list\"]\n";
		 create_edge($$,$1 );
	}
        | expression_list ',' expression
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"expression_list\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"','\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
        ;

unary_operator
    : '-'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"unary_operator\"]\n";
		 fout << "N"<< count <<  " [label = \"'-'\"]\n";
		 create_edge($$,count);
		 count++;
	}
    | '!'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"unary_operator\"]\n";
		 fout << "N"<< count <<  " [label = \"'!'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  ;

selection_statement
    : IF '(' expression ')' statement ELSE statement
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"selection_statement\"]\n";
		 fout << "N"<< count <<  " [label = \"IF\"]\n";
		 create_edge($$,count);
		 count++;
		 fout << "N"<< count <<  " [label = \"'('\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
		 fout << "N"<< count <<  " [label = \"')'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$5 );
		 fout << "N"<< count <<  " [label = \"ELSE\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$7 );
	}
  ;

iteration_statement
	: WHILE '(' expression ')' statement
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"iteration_statement\"]\n";
		 fout << "N"<< count <<  " [label = \"WHILE\"]\n";
		 create_edge($$,count);
		 count++;
		 fout << "N"<< count <<  " [label = \"'('\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
		 fout << "N"<< count <<  " [label = \"')'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$5 );
	}
	| FOR '(' expression ';' expression ';' expression ')' statement 
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"iteration_statement\"]\n";
		 fout << "N"<< count <<  " [label = \"FOR\"]\n";
		 create_edge($$,count);
		 count++;
		 fout << "N"<< count <<  " [label = \"'('\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
		 fout << "N"<< count <<  " [label = \"';'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$5 );
		 fout << "N"<< count <<  " [label = \"';'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$7 );
		 fout << "N"<< count <<  " [label = \"')'\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$9 );
	}
	;

declaration_list
  : declaration
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"declaration_list\"]\n";
		 create_edge($$,$1 );
	}
  | declaration_list declaration
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"declaration_list\"]\n";
		 create_edge($$,$1 );
		 create_edge($$,$2 );
	}
  ;

declaration
  : type_specifier declarator_list ';'
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"declaration\"]\n";
		 create_edge($$,$1 );
		 create_edge($$,$2 );
		 fout << "N"<< count <<  " [label = \"';'\"]\n";
		 create_edge($$,count);
		 count++;
	}
  ;

declarator_list
  : declarator
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"declarator_list\"]\n";
		 create_edge($$,$1 );
	}
  | declarator_list ',' declarator
	{
		 $$=count++;
		 fout << "N" << $$ <<  "[label = \"declarator_list\"]\n";
		 create_edge($$,$1 );
		 fout << "N"<< count <<  " [label = \"','\"]\n";
		 create_edge($$,count);
		 count++;
		 create_edge($$,$3 );
	}
  ;
