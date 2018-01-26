//%debug
%scanner Scanner.h
%scanner-token-function d_scanner.lex()


%polymorphic num: int; fl: float; str: string; stmt: StmtAst*; stmts: list<StmtAst*>*; exp: ExpAst*; exps: list<ExpAst*>*; ar: Arrayref*;

%token VOID INT FLOAT IF ELSE WHILE RETURN FOR
%token <str> OR_OP AND_OP LE_OP GE_OP EQ_OP NE_OP INC_OP
%token <str> IDENTIFIER  
%token <num> INT_CONSTANT 
%token <fl> FLOAT_CONSTANT 
%token <str> STRING_LITERAL
%type <str> unary_operator
%type <stmt> compound_statement  statement assignment_statement selection_statement iteration_statement
%type <stmts> statement_list
%type <exp> expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression
%type <exp> unary_expression postfix_expression primary_expression
%type <exps> expression_list
%type <ar> l_expression
%%
translation_unit
  : function_definition 
  | translation_unit function_definition
  ;

function_definition
  : type_specifier fun_declarator compound_statement
  {
    ($3)->print();
  }
  ;

type_specifier
  : VOID
  | INT
  | FLOAT
  ;

fun_declarator
  : IDENTIFIER '(' parameter_list ')'
  | IDENTIFIER '(' ')'
  ;

parameter_list
  : parameter_declaration
  | parameter_list ',' parameter_declaration
  ;

parameter_declaration
  : type_specifier declarator
  ;

declarator
  : IDENTIFIER
  | declarator '[' constant_expression ']'
  ;

constant_expression
	: INT_CONSTANT
	| FLOAT_CONSTANT
	;

compound_statement
	: '{' '}'
		{
			$$ = new Block_stmt(new list<StmtAst*>);
		} 
	| '{' statement_list '}' 
		{
			$$ = new Block_stmt($2);
		}
    | '{' declaration_list statement_list '}' 
    	{
    		$$ = new Block_stmt($3);
    	}
	;

statement_list
  : statement
  	{
  		$$ = new list<StmtAst*>();
  		($$)->push_back($1);
  	}
  | statement_list statement
  	{
  		($1)->push_back($2);
  		$$ = $1;
  	}
  ;

statement
  : '{' statement_list '}'
  	{
  		$$ = new Block_stmt($2);
  	}
  | selection_statement
  	{
  		$$ = $1;
  	}
  | iteration_statement
  	{
  		$$ = $1;
  	}
  | assignment_statement
  	{
  		$$ = $1;
  	}
  | RETURN expression ';'
  	{
  		$$ = new Return_class($2);
  	}
  ;

assignment_statement
  : ';'
  	{
  		$$ = new Empty();
  	}
  |  l_expression '=' expression ';'
  	 {
  	 	$$ = new Ass($1, $3);
  	 }
  ;

expression
  : logical_and_expression
  	{
  		$$ = $1;
  	}
  | expression OR_OP logical_and_expression
  	{
  		$$ = new Binary_op("OR", $1, $3);
  	}
  ;

logical_and_expression
  : equality_expression
  	{
  		$$ = $1;
  	}
  | logical_and_expression AND_OP equality_expression
  	{
  		$$ = new Binary_op("AND", $1, $3);
  	}
  ;

equality_expression
  : relational_expression
  	{
  		$$ = $1;
  	}
  | equality_expression EQ_OP relational_expression
  	{
  		$$ = new Binary_op("EQ_OP", $1, $3);
  	}
  | equality_expression NE_OP relational_expression
  	{
  		$$ = new Binary_op("NE_OP", $1, $3);
  	}
  ;
relational_expression
  : additive_expression
  	{
  		$$ = $1;
  	}
  | relational_expression '<' additive_expression
  	{
  		$$ = new Binary_op("LT", $1, $3);
  	}
  | relational_expression '>' additive_expression
  	{
  		$$ = new Binary_op("GT", $1, $3);
  	}
  | relational_expression LE_OP additive_expression
  	{
  		$$ = new Binary_op("LE_OP", $1, $3);
  	}
  | relational_expression GE_OP additive_expression
  	{
  		$$ = new Binary_op("GE_OP", $1, $3);
  	}
  ;

additive_expression
  : multiplicative_expression
  	{
  		$$ = $1;
  	}
  | additive_expression '+' multiplicative_expression
  	{
  		$$ = new Binary_op("PLUS", $1, $3);
  	}
  | additive_expression '-' multiplicative_expression
  	{
  		$$ = new Binary_op("MINUS", $1, $3);
  	}
  ;

multiplicative_expression
  : unary_expression
  	{
  		$$ = $1;
  	}
  | multiplicative_expression '*' unary_expression
  	{
  		$$ = new Binary_op("MULT", $1, $3);
  	}
  | multiplicative_expression '/' unary_expression
  	{
  		$$ = new Binary_op("DIV", $1, $3);
  	}
  ;

unary_expression
  : postfix_expression
  | unary_operator postfix_expression
  	{
  		$$ = new Unary_op($1,$2);
  	}
  ;

postfix_expression
  : primary_expression
  	{
  		$$ = $1;
  	}
  | IDENTIFIER '(' ')'
  	{  		
  		$$ = new Fun_call(new list<ExpAst*>());
  	}
  | IDENTIFIER '(' expression_list ')'
  	{
  		$$ = new Fun_call($3);
  	}
  | l_expression INC_OP
  	{
  		$$ = new Unary_op("PP", $1);
  	}
  ;

primary_expression
	: l_expression
		{
			$$ = $1;
		}
    |  l_expression '=' expression  
    	{
  			$$ = new Binary_op("ASSIGN", $1, $3);
  		}
	| INT_CONSTANT
		{
			$$ = new Int_const($1);
		}
	| FLOAT_CONSTANT
		{
			$$ = new Float_const($1);
		}
    | STRING_LITERAL
    	{
    		$$ = new String_const($1);
    	}
	| '(' expression ')'
		{
			$$ = $2;
		}
	;

l_expression
        : IDENTIFIER
        	{
        		$$ = new IdentifierArray($1);
        	}
        | l_expression '[' expression ']' 
        	{
        		$$ = new Index($1,$3);
        	}
        ;
expression_list
        : expression
        	{
        		$$ = new list<ExpAst*>;
        		($$)->push_back($1);
        	}
        | expression_list ',' expression
        	{
        		($1)->push_back($3);
        		$$ = $1;
        	}
        ;

unary_operator
    : '-'
    	{
    		$$ = "UMINUS";
    	}
    | '!'
    	{
    		$$ = "NOT";
    	}
  ;

selection_statement
    : IF '(' expression ')' statement ELSE statement
      {
      	$$ = new If_class($3,$5,$7);
      }
  ;

iteration_statement
	: WHILE '(' expression ')' statement
	  {
	  	$$ = new While_class($3,$5);
	  }
	| FOR '(' expression ';' expression ';' expression ')' statement 
	  {
	  	$$ = new For_class($3,$5,$7,$9);
	  }
	;

declaration_list
  : declaration
  | declaration_list declaration
  ;

declaration
  : type_specifier declarator_list ';'
  ;

declarator_list
  : declarator
  | declarator_list ',' declarator
  ;
