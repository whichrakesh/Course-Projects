// %debug
%scanner Scanner.h
%scanner-token-function d_scanner.lex()


%polymorphic num: int; fl: float; str: string; stmt: StmtAst*; stmts: list<StmtAst*>*; exp: ExpAst*; exps: list<ExpAst*>*; ar: ArrayRefAst*;

%token VOID INT FLOAT IF ELSE WHILE RETURN FOR
%token <str> OR_OP AND_OP LE_OP GE_OP EQ_OP NE_OP INC_OP
%token <str> IDENTIFIER  
%token <num> INT_CONSTANT 
%token <fl> FLOAT_CONSTANT 
%token <str> STRING_LITERAL
%type <str> unary_operator declarator
%type <stmt> compound_statement  statement assignment_statement selection_statement iteration_statement
%type <stmts> statement_list
%type <exp> expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression
%type <exp> unary_expression postfix_expression primary_expression
%type <exps> expression_list
%type <ar> l_expression
%type <num> constant_expression
%%
translation_unit
  : function_definition 
  | translation_unit function_definition
  ;

function_definition
  : type_specifier fun_declarator compound_statement
  {
    lst->dump();
    for(int i=0; i<functions_to_print.size(); i++){
      if(functions_to_print[i] == lst->func_name){
        cout << "AST for function " << lst->func_name << " is\n";
        ($3)->print();
        break;
      }
    }
    genCode("void " + lst->func_name + "(){");
    genCode("\tpushi(ebp); // Setting dynamic link");
  	genCode("\tmove(esp,ebp); // Setting dynamic link");
  	makeSpaceForLocalVariables(lst);
    ($3)->generate_code(lst);
    if(!($3->nextlist).empty()){
    	string label1 = newLabel();    	
    	genCode(label1+":");
    	backpatch($3->nextlist,label1);
    }    
    removeSpaceForLocalVariables(lst);
    genCode("\tloadi(ind(ebp), ebp); // restoring dynamic link");
  	genCode("\tpopi(1); //pop stack");
  	genCode("\treturn; //return");
    genCode("}\n\n");
    //printCode();
    //clearCodeArray();
    lst = NULL;
  }
  ;

type_specifier
  : VOID
  {
    type = new Type(Type::Base,Type::Void);
  }
  | INT
  {
    type = new Type(Type::Base,Type::Int);
  }
  | FLOAT
  {
    type = new Type(Type::Base,Type::Float);
  }
  ;

fun_declarator
  : function_name '(' parameter_list ')'
  {
    offset = 0;
  }
  | function_name '(' ')' 
  {
    offset = 0;
  }
  ;

function_name : IDENTIFIER 
	{
	    offset = 4; 
	    lst = new Local_Symbol_Table;
	    lst->func_name = $1;
	    gst->insert($1,type,lst);
	}
	;
  
parameter_list
  : parameter_declaration
  | parameter_list ',' parameter_declaration
  ;

parameter_declaration
  : type_specifier declarator
  {
    lst->insert($2,type,offset);
    lst->addParameter($2);
    offset += type->getSize();
  }
  ;

declarator
  : IDENTIFIER
  {
    $$ =  $1;    
  }
  | declarator '[' constant_expression ']' 
  {  	
 	if(type->tag == Type::Base){
 		type = new Type(Type::Array,$3,type);
 	} else {
 		Type *prev_type,*temp_type = type;
 		while(temp_type->tag != Type::Base){
 			prev_type = temp_type;
 			temp_type = temp_type->arraytype.basetype;
 		}
 		prev_type->arraytype.basetype = new Type(Type::Array, $3, temp_type);
 	}
    $$ = $1;    
  }
  ;

constant_expression
	: INT_CONSTANT
	{
		$$ = $1;
	}
	// | FLOAT_CONSTANT
	// {
	// 	$$ = $1;
	// }
	;

compound_statement
	: '{' '}'
		{
			$$ = new BlockStmtAst(new list<StmtAst*>);
			$$->setType(new Type(Type::Ok));
		} 
	| '{' statement_list '}' 
		{
			$$ = new BlockStmtAst($2);
		}
    | '{' declaration_list statement_list '}'
    	{
    		$$ = new BlockStmtAst($3);
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
  		$$ = new BlockStmtAst($2);
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
  		// $$ = new ReturnStmtAst($2);
      Type *return_type = gst->table[lst->func_name].return_type;
  		if(lst && compatible(return_type , $2->getType())){
	        ifint(return_type){
	        	iffloat($2->getType())
	          		$$ = new ReturnStmtAst(new ToInt($2));
	          	else
	          		$$ = new ReturnStmtAst($2);
	        } else iffloat(return_type){
	        	ifint($2->getType())
	          		$$ = new ReturnStmtAst(new ToFloat($2));  
	          	else
	          		$$ = new ReturnStmtAst($2);	
	        }
	        $$->setType(new Type(Type::Ok));  			
  		} else {
        	$$ = new ReturnStmtAst($2);
  			parse_error("return type of the function " << lst->func_name << " is different");
  			$$->setType(new Type(Type::Error));
  		}
  	}
  | IDENTIFIER '(' ')' ';'
    {
      $$ = new FunCallStmtAst($1, new list<ExpAst*>());	
      if(gst->table.count($1) > 0){
      	$$->setType( gst->table[$1].return_type);
        // $$ = new FunCallStmtAst($1, new list<ExpAst*>());
      }else{
      	parse_error( "Function " << $1 << " is not defined before calling.");
      	$$->setType(new Type(Type::Error));
      }     
    }   
  | IDENTIFIER '(' expression_list ')' ';'
    {
      $$ = new FunCallStmtAst($1, $3);
      if(gst->table.count($1) > 0){  
        Local_Symbol_Table *temp =  gst->table[$1].lst;
        int i = 0;
        for(auto it = $3->begin(); it != $3->end(); it++){            
            Type *t = temp->getParameterType(i);
            if(t->tag == Type::Error){
              parse_error( "Function call for " << $1 << " has more parameters.");
              $$->setType(new Type(Type::Error));
              return;
            }
            if(!compatible((*it)->getType(),t)){
              parse_error( "Function " << $1 << "'s parameters not compatible with definition.");
              $$->setType(new Type(Type::Error));
              return;
            } else ifint((*it)->getType()){
            	iffloat(t){
            		(*it) = new ToFloat(*it);
            	}
            } else iffloat((*it)->getType()){
            	ifint(t){
            		(*it) = new ToInt(*it);
            	}
            }
            i++;    
        }
        if(i != temp->parameters.size()){
          parse_error( "Function call " << $1 << " has less parameters than the definition.");
          $$->setType(new Type(Type::Error));
        }
        $$ = new FunCallStmtAst($1, $3);
        $$->setType( gst->table[$1].return_type);
      }else{
      	if($1 == "printf"){
      		$$->setType(new Type(Type::Ok));
      	} else {
      		parse_error( "Function " << $1 << " is not defined before calling.");
      		$$->setType(new Type(Type::Error));
      	}
      }     
    }  
  ;

assignment_statement
  : ';'
  	{
  		$$ = new EmptyStmtAst();
  		$$->setType(new Type(Type::Ok));
  	}
  |  l_expression '=' expression ';'
  	 {
  	 	// $$ = new AssignStmtAst($1, $3); 
  	 	$1->to_pass = ArrayRefAst::Address;
  	 	if(compatible($1->getType(),$3->getType())){
	        if($1->getType()->tag == Type::Base &&  $1->getType()->basetype == Type::Int && 
	        	$3->getType()->tag == Type::Base &&  $3->getType()->basetype == Type::Float){
	          $$ = new AssignStmtAst($1, new ToInt($3));          
	        } else if($1->getType()->tag == Type::Base &&  $1->getType()->basetype == Type::Float && 
	        	$3->getType()->tag == Type::Base &&  $3->getType()->basetype == Type::Int){
	          $$ = new AssignStmtAst($1, new ToFloat($3));   
	        } else {
	        	$$ = new AssignStmtAst($1,$3);   
	        }
  	 		// $$ = new AssignStmtAst($1, $3);  	 	
  	 		$$->setType(new Type(Type::Ok));
  	 	}
  	 	else
  	 	{
        	$$ = new AssignStmtAst($1, $3); 
	  	 	parse_error("Type mismatch during assignment");	
	  	 	$$->setType(new Type(Type::Error));
  	 	}
  	 }
  ;

expression
  : logical_and_expression
  	{
  		$$ = $1;
  	}
  | expression OR_OP logical_and_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("OR", $1, $3);
  		if(compatible(t1,$3->getType())){
  	 		$$->setType(new Type(Type::Base,Type::Int));
  	 	}
  	 	else
  	 	{
  	 		parse_error("Type mismatch during logical operation");	
  			$$->setType(new Type(Type::Error));
  		}
  	}
  ;

logical_and_expression
  : equality_expression
  	{
  		$$ = $1;
  	}
  | logical_and_expression AND_OP equality_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("AND", $1, $3);
  		if(compatible(t1,$3->getType())){	 	
  	 		$$->setType(new Type(Type::Base,Type::Int));
  	 	}
  	 	else
  	 	{	
  	 		parse_error("Type mismatch during logical operation");	
  			$$->setType(new Type(Type::Error));
  		}
  	}
  ;

equality_expression
  : relational_expression
  	{
  		$$ = $1;
  	}
  | equality_expression EQ_OP relational_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("EQ_OP", $1, $3);
  		if(compatible(t1,$3->getType())){
  	 		$$->setType(new Type(Type::Base,Type::Int));
  	 	}
  	 	else
  	 	{
  	 		parse_error("Type mismatch during logical operation");	  	 		
  			$$->setType(new Type(Type::Error));
  		}
  	}
  | equality_expression NE_OP relational_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("NE_OP", $1, $3);
  		if(compatible(t1,$3->getType())){
  	 		$$->setType(new Type(Type::Base,Type::Int));
  	 	}
  	 	else
  	 	{
  	 		parse_error("Type mismatch during logical operation");	  	 		
  			$$->setType(new Type(Type::Error));
  		}
  	}
  ;
relational_expression
  : additive_expression
  	{
  		$$ = $1;
  	}
  | relational_expression '<' additive_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("LT", $1, $3);
  		if(compatible(t1,$3->getType())){
  	 		$$->setType(new Type(Type::Base,Type::Int));
  	 	}
  	 	else
  	 	{
  	 		parse_error("Type mismatch during Relational operation");	  	 		
  			$$->setType(new Type(Type::Error));
  		}
  	}
  | relational_expression '>' additive_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("GT", $1, $3);
  		if(compatible(t1,$3->getType())){
  	 		$$->setType(new Type(Type::Base,Type::Int));
  	 	}
  	 	else
  	 	{
  	 		parse_error("Type mismatch during Relational operation");	  	 		
  			$$->setType(new Type(Type::Error));
  		}
  	}
  | relational_expression LE_OP additive_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("LE_OP", $1, $3);
  		if(compatible(t1,$3->getType())){
  	 		$$->setType(new Type(Type::Base,Type::Int));
  	 	}
  	 	else
  	 	{
  	 		parse_error("Type mismatch during Relational operation");	  	 		
  			$$->setType(new Type(Type::Error));
  		}
 	}
  | relational_expression GE_OP additive_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("GE_OP", $1, $3);
  		if(compatible(t1,$3->getType())){
  	 		$$->setType(new Type(Type::Base,Type::Int));
  	 	}
  	 	else
  	 	{
  	 		parse_error("Type mismatch during Relational operation");	  	 		
  			$$->setType(new Type(Type::Error));
  		}  	
  	}
  ;

additive_expression
  : multiplicative_expression
  	{
  		$$ = $1;
  	}
  | additive_expression '+' multiplicative_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("PLUS", $1, $3); 
  		if(compatible(t1,$3->getType())){  						
  		}
  		else
  		{
  			parse_error("type mismatch during plus operation.");	
  			$$->setType(new Type(Type::Error));
  		}
  	}
  | additive_expression '-' multiplicative_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("MINUS", $1, $3);
  		if(compatible(t1,$3->getType())){  			
  		}
  		else{
  			parse_error("type mismatch during minus operation.");	
  			$$->setType(new Type(Type::Error));
  		}	
  	}
  ;

multiplicative_expression
  : unary_expression
  	{
  		$$ = $1;
  	}
  | multiplicative_expression '*' unary_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("MULT", $1, $3);
  		if(compatible(t1,$3->getType())){  			
  		}
  		else{
  			parse_error( "type mismatch during mult operation.");	
  			$$->setType(new Type(Type::Error));
  		}  			
  	}
  | multiplicative_expression '/' unary_expression
  	{
  		Type* t1 = $1->getType();
  		$$ = new BinaryOpAst("DIV", $1, $3);
  		if(compatible(t1,$3->getType())){	  		
	  		$$->setType(resultType($1->getType(),$3->getType()));
  		}
	  	else{
		  	parse_error( "type mismatch during div operation.");	
		  	$$->setType(new Type(Type::Error));
	  	}	
  	}
  ;

unary_expression
  : postfix_expression
  {
  	$$ = $1;
  }
  | unary_operator postfix_expression
  	{
  		if($1 == "NOT"){
  			ifint($2->getType()){
		  		$$ = new UnaryOpAst($1,$2);  				  		
	  		} else iffloat($2->getType()){
	  			ExpAst* exp = new ToInt($2);
	  			$$ = new UnaryOpAst($1, exp); 
	  		} else {
	  			parse_error( "Not a valid expression for the unary operator NOT.");
	  			$$ = new UnaryOpAst($1,$2);	
		  		$$->setType(new Type(Type::Error));
		  		return;
	  		}
	  		$$->setType((Type*) (new Type(Type::Base,Type::Int)));
  		} else {
  			$$ = new UnaryOpAst($1,$2);  		
		  	$$->setType($2->getType());
  		}  		

  	}
  ;

postfix_expression
  : primary_expression
  	{
  		$$ = $1;
  	}
  | IDENTIFIER '(' ')'
  	{  		
      $$ = new FunCallExpAst($1, new list<ExpAst*>()); 
  	  if(gst->table.count($1) > 0){              
        if($$->getType()->tag == Type::Ok){
        	$$->setType( gst->table[$1].return_type);
        }
      }else{
      	parse_error( "Function " << $1 << " is not defined before calling.");
      	$$->setType(new Type(Type::Error));
      } 
  	}
  | IDENTIFIER '(' expression_list ')'
  	{
      $$ = new FunCallExpAst($1, $3);
  	  if(gst->table.count($1) > 0){        
        Local_Symbol_Table *temp =  gst->table[$1].lst;
        int i = 0;
        for(auto it = $3->begin(); it != $3->end(); it++){            
            Type *t = temp->getParameterType(i);
            if(t->tag == Type::Error){
              parse_error( "Function call for " << $1 << " has more parameters.");
              $$->setType(new Type(Type::Error));
              return;
            }
            if(!compatible((*it)->getType(),t)){
              parse_error( "Function " << $1 << "'s parameters not compatible with definition.");
              $$->setType(new Type(Type::Error));
              return;
            } else ifint((*it)->getType()){
            	iffloat(t){
            		(*it) = new ToFloat(*it);
            	}
            } else iffloat((*it)->getType()){
            	ifint(t){
            		(*it) = new ToInt(*it);
            	}
            }
            i++;    
        }
        if(i != temp->parameters.size()){
          parse_error( "Function call " << $1 << " has less parameters than the definition.");
          $$->setType(new Type(Type::Error));
        }
        $$ = new FunCallExpAst($1, $3);
        $$->setType( gst->table[$1].return_type);
      }else{
      	parse_error( "Function " << $1 << " is not defined before calling.");
      	$$->setType(new Type(Type::Error));
      } 
  	}
  | l_expression INC_OP
  	{
  		$1->to_pass = ArrayRefAst::ValueAddress;
  		$$ = new UnaryOpAst("PP", $1);
  		$$->setType( $1->getType());  		
  	}
  ;

primary_expression
	: l_expression
		{
			$$ = $1;
		}
    |  l_expression '=' expression  
    	{
    		Type* t1 = $1->getType();
    		// $$ = new BinaryOpAst("ASSIGN", $1, $3);
    		$1->to_pass = ArrayRefAst::Address;
    		if(compatible($1->getType(),$3->getType())){
	          	if($1->getType()->tag == Type::Base &&  $1->getType()->basetype == Type::Int && 
	        		$3->getType()->tag == Type::Base &&  $3->getType()->basetype == Type::Float){
		          	$$ = new BinaryOpAst("ASSIGN",$1, new ToInt($3));          
		        } else if($1->getType()->tag == Type::Base &&  $1->getType()->basetype == Type::Float && 
		        	$3->getType()->tag == Type::Base &&  $3->getType()->basetype == Type::Int){
		          	$$ = new BinaryOpAst("ASSIGN",$1, new ToFloat($3));   
		        } else {
		        	$$ = new BinaryOpAst("ASSIGN",$1,$3);   
		        }     
	          	$$->setType(t1);
	        } else{
	          	$$ = new BinaryOpAst("ASSIGN", $1, $3);
  				parse_error( "type mismatch during assignment operation.");
  				$$->setType(new Type(Type::Error));
  			}
  		}
	| INT_CONSTANT
		{
			$$ = new IntConst($1);
			$$->setType(new Type(Type::Base,Type::Int));
		}
	| FLOAT_CONSTANT
		{
			$$ = new FloatConst($1);
			$$->setType(new Type(Type::Base,Type::Float));
		}
    | STRING_LITERAL
    	{
    		$$ = new StringConst($1); //todo
        	$$->setType(new Type(Type::Ok,Type::Float));
    	}
	| '(' expression ')'
		{
			$$ = $2;
			$$->setType( $2->getType());
		}
	;

l_expression
        : IDENTIFIER
          {
          	$$ = new ArrayRefIdentifier($1);
            if(lst->table.count($1) > 0){
          		$$->setType( lst->table[$1].type);
          		$$->offset = lst->table[$1].offset;
          	}
            else
            {
             	parse_error( "variable " << $1 << " not declared before use."); 
             	$$->setType(new Type(Type::Error));                	
            }
          }
        | l_expression '[' expression ']' 
        	{
        		Type* t1 = $1->getType();
        		$$ = new Index($1,$3);
        		if(t1->tag == Type::Array){
        			$$->setType(t1->arraytype.basetype);
        			$$->offset = $1->offset;
        		} else {
        			parse_error("Invalid number of indices to array.");
        			$$->setType(new Type(Type::Error));
        		}
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
      	$$ = new IfStmtAst($3,$5,$7);
      	if($3->getType()->tag != Type::Error && 
      		$5->getType()->tag == Type::Ok && $7->getType()->tag == Type::Ok){
      		$$->setType(new Type(Type::Ok));
      	} else {
      		$$->setType(new Type(Type::Error));
      	}
      }
  ;

iteration_statement
	: WHILE '(' expression ')' statement
	  {
	  	$$ = new WhileStmtAst($3,$5);
	  	if($3->getType()->tag != Type::Error && 
      		$5->getType()->tag == Type::Ok){
      		$$->setType(new Type(Type::Ok));
      	} else {
      		$$->setType(new Type(Type::Error));
      	}
	  }
	| FOR '(' expression ';' expression ';' expression ')' statement 
	  {
	  	$$ = new ForStmtAst($3,$5,$7,$9);
	  	if($3->getType()->tag != Type::Error && 
      		$5->getType()->tag != Type::Error && $7->getType()->tag != Type::Error && $9->getType()->tag == Type::Ok){
      		$$->setType(new Type(Type::Ok));
      	} else {
      		$$->setType(new Type(Type::Error));
      	}
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
  {
    offset = offset - type->getSize();
    lst->insert($1,type,offset);    
  }
  | declarator_list ',' declarator
  {
    offset = offset - type->getSize();
    lst->insert($3,type,offset);    
  }
  ;
