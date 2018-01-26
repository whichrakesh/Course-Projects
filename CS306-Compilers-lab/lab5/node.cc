#include <iostream>
#include <vector>
#include <algorithm>

#include "type_check.h"
#include "node.h"

#define ifint(type) if(type->tag == Type::Base && type->basetype == Type::Int)
#define iffloat(type) if(type->tag == Type::Base && type->basetype == Type::Float)
#define perform_relational_int(func) {\
    if(exp1->reg_requirement >= MAX_REG && exp2->reg_requirement >= MAX_REG){ \
        exp1->generate_code(st);            \
        genCode("\tpushi(" + reg_stack.top() + ");"); \
        exp2->generate_code(st); \
        string reg1 = reg_stack.pop(); \
        string reg2 = reg_stack.top(); \
        genCode("\tloadi(ind(esp)," + reg2 + ");"); \
        genCode("\tcmpi("+ reg2 + "," + reg1 + ");"); \
        this->truelist = makelist(global_code_array.size());  \
        genCode("\t" + string(func) + "(_);"); \
        this->falselist = makelist(global_code_array.size()); \
        genCode( "\tj(_);"); \
        reg_stack.push(reg1); \
    } else if(exp1->reg_requirement > exp2->reg_requirement || left_to_right){ \
        reg_stack.swap(); \
        exp1->generate_code(st); \
        string reg2 = reg_stack.pop(); \
        exp2->generate_code(st); \
        string reg1 = reg_stack.top(); \
        genCode("\tcmpi("+ reg2 + "," + reg1 + ");"); \
        this->truelist = makelist(global_code_array.size());  \
        genCode("\t" + string(func) + "(_);"); \
        this->falselist = makelist(global_code_array.size()); \
        genCode( "\tj(_);"); \
        reg_stack.push(reg2); \
        reg_stack.swap(); \
    } else { \
        exp2->generate_code(st); \
        string reg1 = reg_stack.pop(); \
        exp1->generate_code(st); \
        genCode("\tcmpi("+ reg_stack.top() + "," + reg1 + ");"); \
        this->truelist = makelist(global_code_array.size());  \
        genCode("\t" + string(func) + "(_);"); \
        this->falselist = makelist(global_code_array.size()); \
        genCode( "\tj(_);"); \
        reg_stack.push(reg1); \
    }}
    
#define perform_relational_float(func) {\
    if(exp1->reg_requirement >= MAX_REG && exp2->reg_requirement >= MAX_REG){ \
        exp1->generate_code(st);            \
        genCode("\tpushf(" + reg_stack.top() + ");"); \
        exp2->generate_code(st); \
        string reg1 = reg_stack.pop(); \
        string reg2 = reg_stack.top(); \
        genCode("\tloadf(ind(esp)," + reg2 + ");"); \
        genCode("\tcmpf("+ reg2 + "," + reg1 + ");"); \
        this->truelist = makelist(global_code_array.size());  \
        genCode("\t" + string(func) + "(_);"); \
        this->falselist = makelist(global_code_array.size()); \
        genCode( "\tj(_);"); \
        reg_stack.push(reg1); \
    } else if(exp1->reg_requirement > exp2->reg_requirement || left_to_right){ \
        reg_stack.swap(); \
        exp1->generate_code(st); \
        string reg2 = reg_stack.pop(); \
        exp2->generate_code(st); \
        string reg1 = reg_stack.top(); \
        genCode("\tcmpf("+ reg2 + "," + reg1 + ");"); \
        this->truelist = makelist(global_code_array.size());  \
        genCode("\t" + string(func) + "(_);"); \
        this->falselist = makelist(global_code_array.size()); \
        genCode( "\tj(_);"); \
        reg_stack.push(reg2); \
        reg_stack.swap(); \
    } else { \
        exp2->generate_code(st); \
        string reg1 = reg_stack.pop(); \
        exp1->generate_code(st); \
        genCode("\tcmpf("+ reg_stack.top() + "," + reg1 + ");"); \
        this->truelist = makelist(global_code_array.size());  \
        genCode("\t" + string(func) + "(_);"); \
        this->falselist = makelist(global_code_array.size()); \
        genCode( "\tj(_);"); \
        reg_stack.push(reg1); \
    }}

#define perform_arithmatic_int(func) {\
    if(exp1->reg_requirement >= MAX_REG && exp2->reg_requirement >= MAX_REG){ \
        exp1->generate_code(st);            \
        if(!exp1->truelist.empty() && !exp1->falselist.empty()){    \
            string label1 = newLabel(); \
            string label2 = newLabel(); \
            string label3 = newLabel(); \
            genCode(label1 + ":");  \
            genCode("\tmove(1," + reg_stack.top() + ");");  \
            genCode("\tj(" + label3 + ");");    \
            genCode(label2 + ":");  \
            genCode("\tmove(0," + reg_stack.top() + ");");  \
            genCode(label3 + ":");  \
            backpatch(exp1->truelist,label1);   \
            backpatch(exp1->falselist,label2);  \
        }   \
        genCode("\tpushi(" + reg_stack.top() + ");"); \
        exp2->generate_code(st); \
        if(!exp2->truelist.empty() && !exp2->falselist.empty()){    \
            string label1 = newLabel(); \
            string label2 = newLabel(); \
            string label3 = newLabel(); \
            genCode(label1 + ":");  \
            genCode("\tmove(1," + reg_stack.top() + ");");  \
            genCode("\tj(" + label3 + ");");    \
            genCode(label2 + ":");  \
            genCode("\tmove(0," + reg_stack.top() + ");");  \
            genCode(label3 + ":");  \
            backpatch(exp2->truelist,label1);   \
            backpatch(exp2->falselist,label2);  \
        }   \
        string reg1 = reg_stack.pop(); \
        string reg2 = reg_stack.top(); \
        genCode("\tloadi(ind(esp)," + reg2 + ");"); \
        genCode("\t" + string(func) + "(" + reg2 + "," + reg1 + ");"); \
        reg_stack.push(reg1); \
    } else if(exp1->reg_requirement > exp2->reg_requirement || left_to_right){ \
        reg_stack.swap(); \
        exp1->generate_code(st); \
        if(!exp1->truelist.empty() && !exp1->falselist.empty()){    \
            string label1 = newLabel(); \
            string label2 = newLabel(); \
            string label3 = newLabel(); \
            genCode(label1 + ":");  \
            genCode("\tmove(1," + reg_stack.top() + ");");  \
            genCode("\tj(" + label3 + ");");    \
            genCode(label2 + ":");  \
            genCode("\tmove(0," + reg_stack.top() + ");");  \
            genCode(label3 + ":");  \
            backpatch(exp1->truelist,label1);   \
            backpatch(exp1->falselist,label2);  \
        }   \
        string reg2 = reg_stack.pop(); \
        if(exp2->exp_type == Const){    \
            genCode("\tmove(" + to_string(exp2->ivalue) + "," + reg_stack.top() + ");"); \
        }  else {\
            exp2->generate_code(st); \
            if(!exp2->truelist.empty() && !exp2->falselist.empty()){    \
                string label1 = newLabel(); \
                string label2 = newLabel(); \
                string label3 = newLabel(); \
                genCode(label1 + ":");  \
                genCode("\tmove(1," + reg_stack.top() + ");");  \
                genCode("\tj(" + label3 + ");");    \
                genCode(label2 + ":");  \
                genCode("\tmove(0," + reg_stack.top() + ");");  \
                genCode(label3 + ":");  \
                backpatch(exp2->truelist,label1);   \
                backpatch(exp2->falselist,label2);  \
            }   \
        }\
        string reg1 = reg_stack.top(); \
        genCode("\t" + string(func) + "(" + reg2 + "," + reg1 + ");"); \
        reg_stack.push(reg2); \
        reg_stack.swap(); \
    } else { \
        exp2->generate_code(st); \
        if(!exp2->truelist.empty() && !exp2->falselist.empty()){    \
            string label1 = newLabel(); \
            string label2 = newLabel(); \
            string label3 = newLabel(); \
            genCode(label1 + ":");  \
            genCode("\tmove(1," + reg_stack.top() + ");");  \
            genCode("\tj(" + label3 + ");");    \
            genCode(label2 + ":");  \
            genCode("\tmove(0," + reg_stack.top() + ");");  \
            genCode(label3 + ":");  \
            backpatch(exp2->truelist,label1);   \
            backpatch(exp2->falselist,label2);  \
        }   \
        string reg1 = reg_stack.pop(); \
        if(exp1->exp_type == Const){    \
            genCode("\tmove(" + to_string(exp1->ivalue) + "," + reg_stack.top() + ");"); \
        }   else {\
            exp1->generate_code(st); \
            if(!exp1->truelist.empty() && !exp1->falselist.empty()){    \
                string label1 = newLabel(); \
                string label2 = newLabel(); \
                string label3 = newLabel(); \
                genCode(label1 + ":");  \
                genCode("\tmove(1," + reg_stack.top() + ");");  \
                genCode("\tj(" + label3 + ");");    \
                genCode(label2 + ":");  \
                genCode("\tmove(0," + reg_stack.top() + ");");  \
                genCode(label3 + ":");  \
                backpatch(exp1->truelist,label1);   \
                backpatch(exp1->falselist,label2);  \
            }   \
        }\
        genCode("\t" + string(func) + "(" + reg_stack.top() + "," + reg1 + ");"); \
        reg_stack.push(reg1); \
    }} 

#define perform_arithmatic_float(func) {\
    if(exp1->reg_requirement >= MAX_REG && exp2->reg_requirement >= MAX_REG){ \
        exp1->generate_code(st);            \
        genCode("\tpushf(" + reg_stack.top() + ");"); \
        exp2->generate_code(st); \
        string reg1 = reg_stack.pop(); \
        string reg2 = reg_stack.top(); \
        genCode("\tloadf(ind(esp)," + reg2 + ");"); \
        genCode("\t" + string(func) + "(" + reg2 + "," + reg1 + ");"); \
        reg_stack.push(reg1); \
    } else if(exp1->reg_requirement > exp2->reg_requirement || left_to_right){ \
        reg_stack.swap(); \
        exp1->generate_code(st); \
        string reg2 = reg_stack.pop(); \
        if(exp2->exp_type == Const){    \
            genCode("\tmove(" + to_string(exp2->fvalue) + "," + reg_stack.top() + ");"); \
        }  else {\
            exp2->generate_code(st); \
        }\
        string reg1 = reg_stack.top(); \
        genCode("\t" + string(func) + "(" + reg2 + "," + reg1 + ");"); \
        reg_stack.push(reg2); \
        reg_stack.swap(); \
    } else { \
        exp2->generate_code(st); \
        string reg1 = reg_stack.pop(); \
        if(exp1->exp_type == Const){    \
            genCode("\tmove(" + to_string(exp1->fvalue) + "," + reg_stack.top() + ");"); \
        }   else {\
            exp1->generate_code(st); \
        }\
        genCode("\t" + string(func) + "(" + reg_stack.top() + "," + reg1 + ");"); \
        reg_stack.push(reg1); \
    }}

using namespace std;

vector<string> global_code_array;
RegStack reg_stack;
const int MAX_REG = 6;

Type* AbstractASTNode::getType(){
	return type;
}

void AbstractASTNode::setType(Type* type){
	this->type = type;
}

void EmptyStmtAst::print(){
    cout<<"(Empty)";
}
void EmptyStmtAst::generate_code(Local_Symbol_Table *st){    
}

BlockStmtAst::BlockStmtAst(list<StmtAst* >* statement_list){
    this->statement_list = *statement_list;
    //cout<<"rakesh"<<endl;
    for(auto it = statement_list->begin(); it != statement_list->end(); it++){
        if((*it)->getType()->tag == Type::Error){
        	this->setType(new Type(Type::Error));
        	return;
        }        
    }
    this->setType(new Type(Type::Ok));
}
void BlockStmtAst::print(){
    cout<<"(Block\n[";
    list<StmtAst*>::iterator it;
    for(it = statement_list.begin(); it != statement_list.end(); it++){
        (*it)->print();        
        //cout << (*it)->getType()->toString();        
    }
    cout<<"])\n";
}
void BlockStmtAst::generate_code(Local_Symbol_Table *st){  
    auto it = statement_list.begin();  
    for(auto it2 = --statement_list.end(); it != it2; it++){
        (*it)->generate_code(st);
        if(!(*it)->nextlist.empty()){
            string label1 = newLabel();
            genCode(label1 + ":");
            backpatch((*it)->nextlist,label1);
        }        
    }
    (*it)->generate_code(st);
    this->nextlist = (*it)->nextlist;
}

AssignStmtAst::AssignStmtAst(ArrayRefAst *exp1, ExpAst *exp2){
    this->exp1=exp1;
    this->exp2=exp2;
    //exp1->reg_requirement = max(1,exp1->reg_requirement);
    //exp1->to_pass = ArrayRefAst::Address;
}
void AssignStmtAst::print(){
    cout << "( = ";
    exp1->print();
    cout << " "; 
    exp2->print();
    cout<<")\n";
}
void AssignStmtAst::generate_code(Local_Symbol_Table *st){ 
    if(!this->exp2->left_to_right){
        list<ExpAst*> l1 = this->exp2->recomputeExpTree("DUMMY");    
        ExpAst* exp2 = *(l1.begin());
    }
    // (*(l1.begin()))->print();
    // cout << endl;    
    ifint(exp2->getType()){        
        if(exp2->exp_type == ExpAst::Const){ 
            genCode("\tmove(" + to_string(exp2->ivalue) + "," + reg_stack.top() + ");"); \
        } else {           
            exp2->generate_code(st); 
            if(!exp2->truelist.empty() && !exp2->falselist.empty()){
                string label1 = newLabel();
                string label2 = newLabel();
                string label3 = newLabel();
                genCode(label1 + ":");
                genCode("\tmove(1," + reg_stack.top() + ");");
                genCode("\tj(" + label3 + ");");
                genCode(label2 + ":");
                genCode("\tmove(0," + reg_stack.top() + ");");
                genCode(label3 + ":");
                backpatch(exp2->truelist,label1);
                backpatch(exp2->falselist,label2);
            }
        }
        if(exp1->reg_requirement >= MAX_REG){
            genCode("\tpushi(" + reg_stack.top() + ");");
            exp1->generate_code(st);
            string reg1 = reg_stack.pop(); //l_offset
            genCode("\tloadi(ind(esp)," + reg_stack.top() + ");"); // r_exp value
            genCode("\tstorei(" +  reg_stack.top() + ", ind(ebp," + reg1 + "));");
            reg_stack.push(reg1);
        } else {
            string reg1 = reg_stack.pop(); // r_exp value
            exp1->generate_code(st);
            string reg2 = reg_stack.top(); //l_offset
            genCode("\tstorei(" +  reg1 + ", ind(ebp," + reg2 + "));");
            reg_stack.push(reg1);
        }
    } else iffloat(exp2->getType()){        
        if(exp2->exp_type == ExpAst::Const){ 
            genCode("\tmove(" + to_string(exp2->fvalue) + "," + reg_stack.top() + ");"); \
        } else {
            exp2->generate_code(st);
        }       
        if(exp1->reg_requirement >= MAX_REG){
            genCode("\tpushf(" + reg_stack.top() + ");");
            exp1->generate_code(st);
            string reg1 = reg_stack.pop(); //l_offset
            genCode("\tloadf(ind(esp)," + reg_stack.top() + ");"); // r_exp value
            genCode("\tstoref(" +  reg_stack.top() + ", ind(ebp," + reg1 + "));");
            reg_stack.push(reg1);
        } else {
            string reg1 = reg_stack.pop(); // r_exp value
            exp1->generate_code(st);
            string reg2 = reg_stack.top(); //l_offset
            genCode("\tstoref(" +  reg1 + ", ind(ebp," + reg2 + "));");
            reg_stack.push(reg1);
        }
    }
}

ReturnStmtAst::ReturnStmtAst(ExpAst* exp){
    this->exp=exp;
}
void ReturnStmtAst::print(){
    cout<<"(return ";
    exp->print();
    cout<<")\n";
}
void ReturnStmtAst::generate_code(Local_Symbol_Table *st){
    exp->generate_code(st);
    int return_address = st->getReturnOffset();
    ifint(exp->getType()){    
        genCode("\tstorei(" + reg_stack.top() + ", ind(ebp," + to_string(return_address) + ")); //storing return value in stack");
    } else iffloat(exp->getType()){ 
        genCode("\tstoref(" + reg_stack.top() + ", ind(ebp," + to_string(return_address) + ")); //storing return value in stack");        
    }
}

IfStmtAst::IfStmtAst(ExpAst* exp, StmtAst* stmt1, StmtAst* stmt2){
    this->exp = exp;
    this->stmt1 = stmt1;
    this->stmt2 = stmt2;
}
void IfStmtAst::print(){
    cout<<"( if ";
    exp->print();
    cout << " "; 
    stmt1->print();
    cout << " "; 
    stmt2->print();
    cout<<" )\n";
}
void IfStmtAst::generate_code(Local_Symbol_Table *st){
    ifint(exp->getType()){
        genCode("\t// If starts");
        exp->generate_code(st);
        if(exp->truelist.empty() && exp->falselist.empty()){
            genCode("\tcmpi(0,"+ reg_stack.top() +");");
            exp->truelist = makelist(global_code_array.size());
            genCode("\tjne(_);");
            exp->falselist = makelist(global_code_array.size());
            genCode("\tj(_);");
        }
        string label1 = newLabel();
        genCode(label1 + ": //if");
        backpatch(exp->truelist,label1);
        stmt1->generate_code(st);
        list<int> n_list = makelist(global_code_array.size());
        genCode("\tj(_);");
        string label2 = newLabel();
        genCode(label2 + ": // else");
        backpatch(exp->falselist,label2);
        stmt2->generate_code(st);
        this->nextlist = merge(stmt1->nextlist,stmt2->nextlist);
        this->nextlist = merge(this->nextlist,n_list);
        genCode("\t// if ends");
    }
}

WhileStmtAst::WhileStmtAst(ExpAst* exp, StmtAst* stmt){
    this->exp = exp;
    this->stmt = stmt;
}
void WhileStmtAst::print(){
    cout<<"( while ";
    exp->print();
    cout << " "; 
    stmt->print();
    cout<<" )\n";
}
void WhileStmtAst::generate_code(Local_Symbol_Table *st){
    ifint(exp->getType()){
        genCode("\t// while loop starts");
        string label1 = newLabel();
        string label2 = newLabel();
        genCode(label1 + ": //while loop");
        exp->generate_code(st);
        if(exp->truelist.empty() && exp->falselist.empty()){
            genCode("\tcmpi(0,"+ reg_stack.top() +");");
            exp->truelist = makelist(global_code_array.size());
            genCode("\tjne(_);");
            exp->falselist = makelist(global_code_array.size());
            genCode("\tj(_);");
        }
        genCode(label2 + ": //while loop");
        stmt->generate_code(st);
        genCode("\tj(" + label1 + ");");
        backpatch(exp->truelist,label2);
        backpatch(stmt->nextlist,label1);
        this->nextlist = exp->falselist;        
        genCode("\t// while loop ends");
    }
}    

ForStmtAst::ForStmtAst(ExpAst* exp1, ExpAst* exp2, ExpAst* exp3, StmtAst* stmt){
    this->exp1 = exp1;
    this->exp2 = exp2;
    this->exp3 = exp3;
    this->stmt = stmt;
}
void ForStmtAst::print(){
    cout<<"(for ";
    exp1->print();
    cout << " "; 
    exp2->print();
    cout << " "; 
    exp3->print();
    cout << " "; 
    stmt->print();
    cout<<" )";
}
void ForStmtAst::generate_code(Local_Symbol_Table *st){
    ifint(exp2->getType()){
        genCode("\t// for loop starts");
        exp1->generate_code(st);        
        string label1 = newLabel();
        string label2 = newLabel();        
        genCode(label1 + ": // for loop");
        exp2->generate_code(st);
        if(exp2->truelist.empty() && exp2->falselist.empty()){
            genCode("\tcmpi(0,"+ reg_stack.top() +");");
            exp2->truelist = makelist(global_code_array.size());
            genCode("\tjne(_);");
            exp2->falselist = makelist(global_code_array.size());
            genCode("\tj(_);");
        }
        genCode(label2 + ": // for loop");
        stmt->generate_code(st);
        if(!stmt->nextlist.empty()){
            string label3 = newLabel();
            genCode(label3 + ": // for loop");
            backpatch(stmt->nextlist,label3);
        }
        exp3->generate_code(st);
        genCode("\tj(" + label1 + ");");        
        backpatch(exp2->truelist,label2);        
        this->nextlist = exp2->falselist;
        genCode("\t// for loop ends");
    }
}    

BinaryOpAst::BinaryOpAst(string b_op, ExpAst* exp1, ExpAst* exp2){	
    this->b_op = b_op;
    this->exp1 = exp1;
    this->exp2 = exp2;
    this->setType(resultType(exp1->getType(),exp2->getType()));    
    if(this->b_op == "OR" || this->b_op == "AND"){
        iffloat(exp1->getType()){
            this->exp1 = new ToInt(exp1);
        }
        iffloat(exp2->getType()){
            this->exp2 = new ToInt(exp2);
        }
        this->left_to_right = exp1->left_to_right || exp2->left_to_right;
    } else if(this->b_op == "ASSIGN"){
        this->left_to_right = true;
    } else {
        if(exp1->getType()->tag == Type::Base && exp1->getType()->basetype == Type::Int &&
              exp2->getType()->tag == Type::Base && exp2->getType()->basetype == Type::Int) {
           this->b_op = b_op + "_INT";
        } else if(exp1->getType()->tag == Type::Base && exp1->getType()->basetype == Type::Int &&
          exp2->getType()->tag == Type::Base && exp2->getType()->basetype == Type::Float){
            this->b_op = b_op + "_FLOAT";
            this->exp1 = new ToFloat(exp1);
        } else if(exp1->getType()->tag == Type::Base && exp1->getType()->basetype == Type::Float &&
          exp2->getType()->tag == Type::Base && exp2->getType()->basetype == Type::Int){
            this->b_op = b_op + "_FLOAT";
            this->exp2 = new ToFloat(exp2);
        } else if(exp1->getType()->tag == Type::Base && exp1->getType()->basetype == Type::Float &&
            exp2->getType()->tag == Type::Base && exp2->getType()->basetype == Type::Float){
            this->b_op = b_op + "_FLOAT";          
        }  
        this->left_to_right = exp1->left_to_right || exp2->left_to_right;
    }
    
    if(this->b_op == "ASSIGN"){
        //exp1->reg_requirement = max(exp1->reg_requirement,2);
        //exp1->to_pass = ArrayRefAst::Address;
        this->reg_requirement = max(exp1->reg_requirement, 1 + exp2->reg_requirement);
    } else if(this->b_op == "OR" || this->b_op == "AND") {
        this->reg_requirement = max(exp1->reg_requirement,exp2->reg_requirement);
    } else {
        if(exp1->reg_requirement == exp2->reg_requirement)
            this->reg_requirement = 1+exp1->reg_requirement;
        else 
            this->reg_requirement = max(exp1->reg_requirement,exp2->reg_requirement);            
    }

    if(exp1->exp_type == ExpAst::Const && exp1->exp_type == ExpAst::Const){
        this->exp_type = Const;
        this->reg_requirement = 1;
        if(this->b_op == "PLUS_INT"){
            this->ivalue = this->exp1->ivalue + this->exp2->ivalue;
        } else if(this->b_op == "PLUS_FLOAT"){
            this->fvalue = this->exp1->fvalue + this->exp2->fvalue;
        } else if(this->b_op == "MINUS_INT"){
            this->ivalue = this->exp1->ivalue - this->exp2->ivalue;
        } else if(this->b_op == "MINUS_FLOAT"){
            this->fvalue = this->exp1->fvalue - this->exp2->fvalue;
        } else if(this->b_op == "MULT_INT"){
            this->ivalue = this->exp1->ivalue * this->exp2->ivalue;
        } else if(this->b_op == "MULT_FLOAT"){
            this->fvalue = this->exp1->fvalue * this->exp2->fvalue;
        } else if(this->b_op == "DIV_INT"){
            this->ivalue = this->exp1->ivalue / this->exp2->ivalue;
        } else if(this->b_op == "DIV_FLOAT"){
            this->fvalue = this->exp1->fvalue / this->exp2->fvalue;
        }        
    }
}
void BinaryOpAst::print(){
    cout<<"( " << b_op << " ";
    exp1->print();
    cout << " ";
    exp2->print();
    cout<<" )";
}
//TODO
void BinaryOpAst::generate_code(Local_Symbol_Table *st){
    if(this->b_op == "PLUS_INT"){
        perform_arithmatic_int("addi");        
    } else if(this->b_op == "PLUS_FLOAT"){
        perform_arithmatic_float("addf");
    } else if(this->b_op == "MINUS_INT"){
        if(exp1->reg_requirement >= MAX_REG && exp2->reg_requirement >= MAX_REG){
            exp1->generate_code(st);            
            genCode("\tpushi(" + reg_stack.top() + ");");
            exp2->generate_code(st);
            string reg1 = reg_stack.pop(); //exp2
            string reg2 = reg_stack.top();
            genCode("\tloadi(ind(esp)," + reg2 + ");"); // loading exp1 into reg2
            genCode("\tmuli(-1," + reg1 +");"); // exp2 * -1
            genCode("\taddi(" + reg2 + "," + reg1 + ");");
            reg_stack.push(reg1); // restoring stack
        } else if(exp1->reg_requirement > exp2->reg_requirement){
            reg_stack.swap();
            exp1->generate_code(st);
            string reg2 = reg_stack.pop(); // exp1
            exp2->generate_code(st);
            string reg1 = reg_stack.top(); // exp2
            genCode("\tmuli(-1," + reg1 +");");
            genCode("\taddi(" + reg2 + "," + reg1 + ");");
            reg_stack.push(reg2);
            reg_stack.swap(); //restore stack
        } else {
            exp2->generate_code(st);
            string reg1 = reg_stack.pop();
            exp1->generate_code(st);
            genCode("\tmuli(-1," + reg1 +");");
            genCode("\taddi(" + reg_stack.top() + "," + reg1 + ");");
            reg_stack.push(reg1);
        }
    } else if(this->b_op == "MINUS_FLOAT"){
        if(exp1->reg_requirement >= MAX_REG && exp2->reg_requirement >= MAX_REG){
            exp1->generate_code(st);            
            genCode("\tpushf(" + reg_stack.top() + ");");
            exp2->generate_code(st);
            string reg1 = reg_stack.pop();
            string reg2 = reg_stack.top();
            genCode("\tloadf(ind(esp)," + reg2 + ");"); // loading exp1 into reg2
            genCode("\tmulf(-1," + reg1 +");");
            genCode("\taddf(" + reg2 + "," + reg1 + ");");
            reg_stack.push(reg1); // restoring stack
        } else if(exp1->reg_requirement > exp2->reg_requirement){
            reg_stack.swap();
            exp1->generate_code(st);
            string reg2 = reg_stack.pop();
            exp2->generate_code(st);
            string reg1 = reg_stack.top();
            genCode("\tmulf(-1," + reg1 +");");
            genCode("\taddf(" + reg2 + "," + reg1 + ");");
            reg_stack.push(reg2);
            reg_stack.swap(); //restore stack
        } else {
            exp2->generate_code(st);
            string reg1 = reg_stack.pop();
            exp1->generate_code(st);
            genCode("\tmulf(-1," + reg1 +");");
            genCode("\taddf(" + reg_stack.top() + "," + reg1 + ");");
            reg_stack.push(reg1);
        }
    } else if(this->b_op == "MULT_INT"){
        perform_arithmatic_int("muli");
    } else if(this->b_op == "MULT_FLOAT"){
        perform_arithmatic_float("mulf");
    } else if(this->b_op == "DIV_INT"){
        perform_arithmatic_int("divi");
    } else if(this->b_op == "DIV_FLOAT"){
        perform_arithmatic_float("divf");
    } else if(this->b_op == "OR"){
        exp1->generate_code(st); 
        if(exp1->truelist.empty() && exp1->falselist.empty()){
            genCode("\tcmpi(0,"+ reg_stack.top() +");");
            exp1->truelist = makelist(global_code_array.size());
            genCode("\tjne(_);");
            exp1->falselist = makelist(global_code_array.size());
            genCode("\tj(_);");
        }
        string label1 = newLabel();
        genCode(label1 + ":");
        exp2->generate_code(st);
        if(exp2->truelist.empty() && exp2->falselist.empty()){
            genCode("\tcmpi(0,"+ reg_stack.top() +");");
            exp2->truelist = makelist(global_code_array.size());
            genCode("\tjne(_);");
            exp2->falselist = makelist(global_code_array.size());
            genCode("\tj(_);");
        }
        backpatch(exp1->falselist,label1);
        this->truelist = merge(exp1->truelist,exp2->truelist);
        this->falselist = exp2->falselist;
    } else if(this->b_op == "AND"){
        exp1->generate_code(st); 
        if(exp1->truelist.empty() && exp1->falselist.empty()){
            genCode("\tcmpi(0,"+ reg_stack.top() +");");
            exp1->truelist = makelist(global_code_array.size());
            genCode("\tjne(_);");
            exp1->falselist = makelist(global_code_array.size());
            genCode("\tj(_);");
        }
        string label1 = newLabel();
        genCode(label1 + ":");
        exp2->generate_code(st);
        if(exp2->truelist.empty() && exp2->falselist.empty()){
            genCode("\tcmpi(0,"+ reg_stack.top() +");");
            exp2->truelist = makelist(global_code_array.size());
            genCode("\tjne(_);");
            exp2->falselist = makelist(global_code_array.size());
            genCode("\tj(_);");
        }
        backpatch(exp1->truelist,label1);
        this->falselist = merge(exp1->falselist,exp2->falselist);
        this->truelist = exp2->truelist;
    } else if(this->b_op == "ASSIGN"){
        ifint(exp2->getType()){
            if(exp2->exp_type == ExpAst::Const){ 
                genCode("\tmove(" + to_string(exp2->ivalue) + "," + reg_stack.top() + ");"); \
            } else {  
                exp2->generate_code(st);
                if(!exp2->truelist.empty() && !exp2->falselist.empty()){
                    string label1 = newLabel();
                    string label2 = newLabel();
                    string label3 = newLabel();
                    genCode(label1 + ":");
                    genCode("\tmove(1," + reg_stack.top() + ");");
                    genCode("\tj(" + label3 + ");");
                    genCode(label2 + ":");
                    genCode("\tmove(0," + reg_stack.top() + ");");
                    genCode(label3 + ":");
                    backpatch(exp2->truelist,label1);
                    backpatch(exp2->falselist,label2);
                }
            }
            if(exp1->reg_requirement >= MAX_REG){
                genCode("\tpushi(" + reg_stack.top() + ");");
                exp1->generate_code(st);
                string reg1 = reg_stack.pop(); //l_offset
                genCode("\tloadi(ind(esp)," + reg_stack.top() + ");"); // r_exp value
                genCode("\tstorei(" +  reg_stack.top() + ", ind(ebp," + reg1 + "));");
                reg_stack.push(reg1);
            } else {
                string reg1 = reg_stack.pop(); // r_exp value
                exp1->generate_code(st);
                string reg2 = reg_stack.top(); //l_offset
                genCode("\tstorei(" +  reg1 + ", ind(ebp," + reg2 + "));");
                reg_stack.push(reg1);
            }
        } else iffloat(exp2->getType()){
            if(exp2->exp_type == ExpAst::Const){ 
                genCode("\tmove(" + to_string(exp2->fvalue) + "," + reg_stack.top() + ");"); \
            } else {
                exp2->generate_code(st);
            }  
            if(exp1->reg_requirement >= MAX_REG){
                genCode("\tpushf(" + reg_stack.top() + ");");
                exp1->generate_code(st);
                string reg1 = reg_stack.pop(); //l_offset
                genCode("\tloadf(ind(esp)," + reg_stack.top() + ");"); // r_exp value
                genCode("\tstoref(" +  reg_stack.top() + ", ind(ebp," + reg1 + "));");
                reg_stack.push(reg1);
            } else {
                string reg1 = reg_stack.pop(); // r_exp value
                exp1->generate_code(st);
                string reg2 = reg_stack.top(); //l_offset
                genCode("\tstoref(" +  reg1 + ", ind(ebp," + reg2 + "));");
                reg_stack.push(reg1);
            }
        }
    } else if(this->b_op == "EQ_OP_INT"){
        perform_relational_int("je");
    } else if(this->b_op == "EQ_OP_FLOAT"){
        perform_relational_float("je");
    } else if(this->b_op == "NE_OP_INT"){
        perform_relational_int("jne");
    } else if(this->b_op == "NE_OP_FLOAT"){
        perform_relational_float("jne");
    } else if(this->b_op == "LT_INT"){
        perform_relational_int("jl");
    } else if(this->b_op == "LT_FLOAT"){
        perform_relational_float("jl");
    } else if(this->b_op == "GT_INT"){
        perform_relational_int("jg");
    } else if(this->b_op == "GT_FLOAT"){
        perform_relational_float("jg");
    } else if(this->b_op == "LE_OP_INT"){
        perform_relational_int("jle");
    } else if(this->b_op == "LE_OP_FLOAT"){
        perform_relational_float("jle");
    } else if(this->b_op == "GE_OP_INT"){
        perform_relational_int("jge");
    } else if(this->b_op == "GE_OP_INT"){
        perform_relational_float("jge");
    }
}
list<ExpAst*> BinaryOpAst::recomputeExpTree(string op){
    if(this->b_op == op){
        list<ExpAst*> l1 = exp1->recomputeExpTree(op);
        list<ExpAst*> l2 = exp2->recomputeExpTree(op);
        l1.merge(l2);
        return l1; 
    } else if(this->b_op.find("PLUS")==0 || this->b_op.find("MULT")==0 || this->b_op.find("OR")==0 || this->b_op.find("AND")==0){
        list<ExpAst*> l1 = exp1->recomputeExpTree(this->b_op);
        list<ExpAst*> l2 = exp2->recomputeExpTree(this->b_op);
        l1.merge(l2);
        l1.sort(compareFunction);
        string op;
        op = getRawOp(this->b_op);
        list<ExpAst*>::iterator it = l1.begin();
        ExpAst* lhs = *(it++);
        for( ;it != l1.end(); it++){
            lhs = new BinaryOpAst(op, lhs, *it);
        }
        list<ExpAst*> l3;
        l3.push_back(lhs);
        return l3;
    } else {
        list<ExpAst*> l1 = exp1->recomputeExpTree("DUMMY");
        list<ExpAst*> l2 = exp2->recomputeExpTree("DUMMY");
        list<ExpAst*> l3;
        op = getRawOp(this->b_op);
        l3.push_back(new BinaryOpAst(op,*(l1.begin()),*(l2.begin())));
        return l3;
    }

}

UnaryOpAst::UnaryOpAst(string u_op, ExpAst* exp){
    this->u_op = u_op;
    this->exp = exp;
    this->reg_requirement = exp->reg_requirement;
    if(u_op == "PP"){
        exp->reg_requirement = max(2,exp->reg_requirement);
        this->left_to_right = true;
    }
}
void UnaryOpAst::print(){
    cout<<"( "<< u_op;
    exp->print();
    cout << " )";
}
void UnaryOpAst::generate_code(Local_Symbol_Table *st){
    if(this->u_op == "UMINUS"){
        ifint(exp->getType()){
            exp->generate_code(st);
            genCode("\tmuli(-1, " + reg_stack.top() + ");");
        } else iffloat(exp->getType()){
            exp->generate_code(st);
            genCode("\tmulf(-1, " + reg_stack.top() + ");");
        }
    }else if(this->u_op == "NOT"){
        exp->generate_code(st);
        if(exp->truelist.empty() && exp->falselist.empty()){
            genCode("\tcmpi(0,"+ reg_stack.top() +");");
            exp->truelist = makelist(global_code_array.size());
            genCode("\tjne(_);");
            exp->falselist = makelist(global_code_array.size());
            genCode("\tj(_);");
        }
        this->truelist = exp->falselist;
        this->falselist = exp->truelist;
    } else if(this->u_op == "PP"){
        ifint(exp->getType()){
            exp->generate_code(st);
            string reg1 = reg_stack.pop(); // exp value
            string reg2 = reg_stack.top(); // offset                        
            genCode("\taddi(1, " + reg1 + ");");
            genCode("\tstorei(" + reg1 + "," + " ind(ebp," + reg2 + "));"); // changing the value of the variable
            genCode("\taddi(-1, " + reg1 + ");");
            reg_stack.push(reg1); //restoring stack
        } else iffloat(exp->getType()){
            exp->generate_code(st);
            string reg1 = reg_stack.pop(); // exp value
            string reg2 = reg_stack.top(); // offset                        
            genCode("\taddf(1, " + reg1 + ");");
            genCode("\tstoref(" + reg1 + "," + " ind(ebp," + reg2 + "));"); // changing the value of the variable
            genCode("\taddf(-1, " + reg1 + ");");
            reg_stack.push(reg1); //restoring stack
        }
    }
}

list<ExpAst*> UnaryOpAst::recomputeExpTree(string op){
    list<ExpAst*> l1 = exp->recomputeExpTree("DUMMY");
    *(l1.begin()) = new UnaryOpAst(this->u_op,*(l1.begin()));
    return l1;
}

FunCallStmtAst::FunCallStmtAst(string name, list<ExpAst* >* expression_list){
    this->func_name = name;
    this->expression_list = *expression_list;
    for(auto it = expression_list->begin(); it != expression_list->end(); it++){
        if((*it)->getType()->tag == Type::Error){
            this->setType(new Type(Type::Error));
            return;
        }
    }
    this->setType(new Type(Type::Ok));
}
void FunCallStmtAst::print(){
    list<ExpAst*>::iterator it;
    cout << "( FunCallStmtAst " << func_name << " ";
    for(it = expression_list.begin(); it != expression_list.end(); it++){
        (*it)->print();
        cout<<" ";
    }
    cout << ")" << endl;
}
void FunCallStmtAst::generate_code(Local_Symbol_Table *st){
    if(func_name != "printf"){
        ifint(this->getType()){
            genCode("\tpushi(0); //passing return value"); //space for return value
        } else iffloat(this->getType()){
            genCode("\tpushf(0); //passing return value");
        }
        int offset = 0;
        list<ExpAst*> passing_list = expression_list;
        reverse(passing_list.begin(),passing_list.end());
        for(auto it = passing_list.begin(); it != passing_list.end(); it++){
            (*it)->generate_code(st); // generate code and push values into stack
            ifint((*it)->getType()){
                 offset += sizeof(int);
                 genCode("\tpushi(" + reg_stack.top() + ");");
            } else iffloat((*it)->getType()){
                 offset += sizeof(float);
                 genCode("\tpushf(" + reg_stack.top() + ");");
            }
        }
        genCode("\t" + func_name + "();");
        if(offset > 0)
            genCode("\taddi(" + to_string(offset) + ", esp); //restoring stack");
        ifint(this->getType()){ // cleaning up return value in the stack
            genCode("\tpopi(1); //cleaning return value"); //space for return value
        } else iffloat(this->getType()){
            genCode("\tpopf(1); //cleaning return value");
        }
    } else {        
        for(auto it = expression_list.begin(); it != expression_list.end(); it++){
            (*it)->generate_code(st);
            ifint((*it)->getType()){
                genCode("\tprint_int(" + reg_stack.top() + ");");
                genCode("\tprint_char(' ');" );
            } else iffloat((*it)->getType()){
                genCode("\tprint_float(" + reg_stack.top() + ");");
                genCode("\tprint_char(' ');" );
            } else if((*it)->getType()->tag == Type::Ok){
                           
            } else {
                cerr << "unexpected parameter given to function print.";
            }
        }            
        genCode("\tprint_char('\\n');" );
    }    
}

FunCallExpAst::FunCallExpAst(string name, list<ExpAst* >* expression_list){
    this->func_name = name;
    this->expression_list = *expression_list;
    int reg_req = 0;
    for(auto it = this->expression_list.begin(); it != this->expression_list.end(); it++){
        if((*it)->getType()->tag == Type::Error){
            this->setType(new Type(Type::Error));
            return;
        }
        reg_req = max(reg_req,(*it)->reg_requirement);
    }
    this->reg_requirement = reg_req;
    this->setType(new Type(Type::Ok));    
}
void FunCallExpAst::print(){
    list<ExpAst*>::iterator it;
    cout << "( FunCallExpAst " << func_name << " ";
    for(it = expression_list.begin(); it != expression_list.end(); it++){
        (*it)->print();
        cout<<" ";
    }
    cout << ")";
}
void FunCallExpAst::generate_code(Local_Symbol_Table *st){
    list<string> used_regs = reg_stack.getUsedRegisters();
    string return_reg = reg_stack.top();
    for(auto it = used_regs.begin(); it != used_regs.end(); it++){ //save used registers
        genCode("\tpushi(" + *it + "); // save used regs"); // kaam chalau, don't know their types
        reg_stack.push(*it);
    }
    ifint(this->getType()){
        genCode("\tpushi(0); //passing return value"); //space for return value
    } else iffloat(this->getType()){
        genCode("\tpushf(0); //passing return value");
    }
    int offset = 0;
    list<ExpAst*> passing_list = expression_list;
    reverse(passing_list.begin(),passing_list.end());
    for(auto it = passing_list.begin(); it != passing_list.end(); it++){
        (*it)->generate_code(st); // generate parameter and push values into stack
        ifint((*it)->getType()){
             offset += sizeof(int);
             genCode("\tpushi(" + reg_stack.top() + ");");
        } else iffloat((*it)->getType()){
             offset += sizeof(float);
             genCode("\tpushf(" + reg_stack.top() + ");");
        }
    }
    genCode("\t" + func_name + "();");
    if(offset > 0)
        genCode("\taddi(" + to_string(offset) + ", esp); //restoring stack");        

    ifint(this->getType()){ // getting return value from the stack
        genCode("\tloadi(ind(esp), " + return_reg + ");"); //space for return value
        genCode("\tpopi(1); // getting return value from stack");
    } else iffloat(this->getType()){
        genCode("\tloadf(ind(esp), " + return_reg + ");"); //space for return value
        genCode("\tpopf(1); //getting return value");
    }

    reverse(used_regs.begin(),used_regs.end()); // getting in reverse order
    for(auto it = used_regs.begin(); it != used_regs.end(); it++){ //restore used registers
        genCode("\tloadi(ind(esp)," + *it + "); // get used regs"); // kaam chalau, don't know their types
        genCode("\tpopi(1);");
        reg_stack.pop(); //removing stored registers from stack
    }
}

list<ExpAst*> FunCallExpAst::recomputeExpTree(string op){
    // list<ExpAst*>* exp_list = new list<ExpAst*>();
    list<ExpAst*> l1;
    /*for(auto it = expression_list.begin(); it != expression_list.end(); it++){
        l1 = (*it)->recomputeExpTree("DUMMY");
        exp_list->push_back(*(l1.begin()));        
    }
    l1.clear();*/
    l1.push_back(this);
    // l1.push_back(new FunCallExpAst(this->func_name,exp_list));
    return l1;
}

FloatConst::FloatConst(float value){
    this->value = value;
    this->reg_requirement = 1;
    this->exp_type = ExpAst::Const;
    this->fvalue = value;    
}
void FloatConst::print(){
    cout<<"( FloatConst "<<value<<" )";
}
void FloatConst::generate_code(Local_Symbol_Table *st){
    genCode("\tmove(" + to_string(value) + "," + reg_stack.top() + ");");
}
list<ExpAst*> FloatConst::recomputeExpTree(string op){   
    list<ExpAst*> l1;
    l1.push_back(this);
    return l1;
}

IntConst::IntConst(int value){
    this->value = value;
    this->reg_requirement = 1;
    this->exp_type = ExpAst::Const;
    this->ivalue = value; 
}
void IntConst::print(){
    cout<<"( IntConst "<<value<<" )";
}
void IntConst::generate_code(Local_Symbol_Table *st){
    genCode("\tmove(" + to_string(value) + "," + reg_stack.top() + ");");
}
list<ExpAst*> IntConst::recomputeExpTree(string op){
    list<ExpAst*> l1;
    l1.push_back(this);
    return l1;
}

StringConst::StringConst(string value){
    this->value = value;
    this->reg_requirement = 1;
}
void StringConst::print(){
    cout<<"( StringLiteral \""<<value<<"\" )";
}
//TODO
void StringConst::generate_code(Local_Symbol_Table *st){   
    genCode("\tprint_string(" + value + ");");
    //genCode("\tmove(var" + to_string(var_no) + "," + reg_stack.top() + ");");
}
list<ExpAst*> StringConst::recomputeExpTree(string op){
    list<ExpAst*> l1;
    l1.push_back(this);
    return l1;
}

Identifier::Identifier(string value){
    this->value = value;
}
void Identifier::print(){
    cout<<"(Id: "<<value<<" )";
} 

void Identifier::generate_code(Local_Symbol_Table *st){
    //pushi(this->value);
}
list<ExpAst*> Identifier::recomputeExpTree(string op){
    list<ExpAst*> l1;
    l1.push_back(this);
    return l1;
}

ToInt::ToInt(ExpAst *exp){
    this->exp = exp;
    this->setType(new Type(Type::Base, Type::Int));
    this->reg_requirement = exp->reg_requirement;
    this->left_to_right = exp->left_to_right;
    if(exp->exp_type == Const){
        this->exp_type = Const;
        this->ivalue = (int) exp->fvalue;
        //cerr << exp->fvalue << " " << ivalue << endl;
    }
}
void ToInt::print(){
    cout << "(TO_INT ";
    exp->print();
    cout << " )";
}
void ToInt::generate_code(Local_Symbol_Table *st){
    //pushi(this->value);    
    exp->generate_code(st);
    genCode("\tfloatToint(" + reg_stack.top() + ");");
}
list<ExpAst*> ToInt::recomputeExpTree(string op){
    list<ExpAst*> l1 = exp->recomputeExpTree("DUMMY");
    *(l1.begin()) = new ToInt(*(l1.begin()));
    return l1;
}
ToFloat::ToFloat(ExpAst *exp){
    this->exp = exp;
    this->setType(new Type(Type::Base, Type::Float));
    this->reg_requirement = exp->reg_requirement;
    this->left_to_right = exp->left_to_right;
    if(exp->exp_type == Const){
        this->exp_type = Const;
        this->fvalue = (float) exp->ivalue;
        //cerr << fvalue;
    }
}
void ToFloat::print(){
    cout << "(TO_FLOAT ";
    exp->print();
    cout << " )";
}
void ToFloat::generate_code(Local_Symbol_Table *st){
    //pushi(this->value);
    exp->generate_code(st);
    genCode("\tintTofloat(" + reg_stack.top() + ");");
}
list<ExpAst*> ToFloat::recomputeExpTree(string op){
    list<ExpAst*> l1 = exp->recomputeExpTree("DUMMY");
    *(l1.begin()) = new ToFloat(*(l1.begin()));
    return l1;
}

ArrayRefIdentifier::ArrayRefIdentifier(string value){
    this->value = value;
    this->reg_requirement = 1;
}
void ArrayRefIdentifier::print(){
    cout<<"( Identifier "<<value << " )";
}
void ArrayRefIdentifier::generate_code(Local_Symbol_Table *st){
    ifint(this->getType()){
        if(this->to_pass == Value){
            genCode("\tloadi(ind(ebp," + to_string(offset) + ")," + reg_stack.top() + "); //loading " + value +" from memory" );
        } else if(this->to_pass == ValueAddress){
            genCode("\tloadi(ind(ebp," + to_string(offset) + ")," + reg_stack.top() + "); //loading " + value +" from memory" );
            string reg1 = reg_stack.pop();
            genCode("\tmove("+ to_string(offset) + "," + reg_stack.top() + ");");
            reg_stack.push(reg1);
        } else if(this->to_pass == Address){
            genCode("\tmove("+ to_string(offset) + "," + reg_stack.top() + ");");
        }                
    } else iffloat(this->getType()){
        if(this->to_pass == Value){
            genCode("\tloadf(ind(ebp," + to_string(offset) + ")," + reg_stack.top() + "); //loading " + value +" from memory" );
        } else if(this->to_pass == ValueAddress){
            string reg1 = reg_stack.pop();
            genCode("\tmove("+ to_string(offset) + "," + reg_stack.top() + ");");
            reg_stack.push(reg1);
        } else if(this->to_pass == Address){
            genCode("\tmove("+ to_string(offset) + "," + reg_stack.top() + ");");
        }
    } else {
        genCode("\tmove(" + to_string(offset) + "," + reg_stack.top() + "); //adding base offset to edx");
    }
}
list<ExpAst*> ArrayRefIdentifier::recomputeExpTree(string op){
    list<ExpAst*> l1;
    l1.push_back(this);
    return l1;
}

Index::Index(ArrayRefAst* array_ref, ExpAst* exp){
    this->array_ref = array_ref;
    iffloat(exp->getType()){
        this->exp = new ToInt(exp);
    } else ifint(exp->getType()){
        this->exp = exp;
    } else {
        cerr << "Invalid Array Index. string given" << endl; 
        exit(-1);
    }
    
    this->reg_requirement = max(array_ref->reg_requirement,1+exp->reg_requirement);
}
void Index::print(){
    cout << "( ArrayIndex ";
    array_ref->print();
    cout << " ";
    exp->print();
    cout<<" )";
}
//TODO
void Index::generate_code(Local_Symbol_Table *st){
    //pushi(this->value);
    array_ref->generate_code(st);
    if(exp->reg_requirement >= MAX_REG){
        genCode("\tpushi(" + reg_stack.top() + ");");        
        exp->generate_code(st);
        string reg1 = reg_stack.pop(); //expression
        string reg2 = reg_stack.top();        
        genCode("\tloadi(ind(esp)," + reg2  + ");"); //prev offset
        Type *this_type = this->getType();
        int size = this_type->getSize();
        genCode("\tmuli(" + to_string(size) + ","+ reg1 + ");"); // new offset
        genCode("\taddi(" + reg2 + "," + reg1 + ");"); 
        reg_stack.push(reg1); //restoring stack
    }else{
        string reg1 = reg_stack.pop(); // prev offset
        exp->generate_code(st);
        string reg2 = reg_stack.top(); // expression
        Type *this_type = this->getType();
        int size = this_type->getSize();
        genCode("\tmuli(" + to_string(size) + ","+ reg2 + ");");
        genCode("\taddi(" + reg2 + "," + reg1 + ");"); //new offset
        reg_stack.push(reg1); //restoring stack
    }
    ifint(this->getType()){
        if(this->to_pass == Value){
            genCode("\tloadi(ind(ebp," + reg_stack.top() + ")," + reg_stack.top() + ");");
        } else if(this->to_pass == ValueAddress){
            string reg1 = reg_stack.pop();
            genCode("\tmove(" + reg1 + "," + reg_stack.top() + ");");
            genCode("\tloadi(ind(ebp," + reg_stack.top() + ")," + reg1 + ");");
            reg_stack.push(reg1); //restoring stack
        } else if(this->to_pass == Address){

        }
    } else iffloat(this->getType()){
        if(this->to_pass == Value){
            genCode("\tloadf(ind(ebp," + reg_stack.top() + ")," + reg_stack.top() + ");");
        } else if(this->to_pass == ValueAddress){
            string reg1 = reg_stack.pop();
            genCode("\tmove(" + reg1 + "," + reg_stack.top() + ");");
            genCode("\tloadf(ind(ebp," + reg_stack.top() + ")," + reg1 + ");");
            reg_stack.push(reg1); //restoring stack 
        } else if(this->to_pass == Address){
        }          
    }
}
list<ExpAst*> Index::recomputeExpTree(string op){
    //list<ExpAst*> l1 = array_ref->recomputeExpTree("DUMMY");
    list<ExpAst*> l2 = exp->recomputeExpTree("DUMMY");
    //list<ExpAst*> l3;
    *(l2.begin()) = new Index(array_ref,*(l2.begin()));
    return l2;
}

RegStack::RegStack(){
    r_stack.push("esi");
    r_stack.push("edi");
    r_stack.push("edx");
    r_stack.push("ecx");
    r_stack.push("ebx");
    r_stack.push("eax");
}

void RegStack::push(string reg){
    r_stack.push(reg);
    used_regs.remove(reg);
}

void RegStack::swap(){
    string top1 = r_stack.top();
    r_stack.pop();
    string top2 = r_stack.top();
    r_stack.pop();
    r_stack.push(top1);
    r_stack.push(top2);
}

string RegStack::top(){
    return r_stack.top();
}

string RegStack::pop(){
    string top = r_stack.top();
    r_stack.pop();
    used_regs.push_back(top);
    return top;
}

int RegStack::size(){
    return r_stack.size();
}

list<string> RegStack::getUsedRegisters(){
    return used_regs;
}


void genCode(string s){
    global_code_array.push_back(s);
//    cout << s << endl;
}

void printCode(){
    ofstream fout;
    fout.open("code.asm",ios::out);
    for(int i=0; i<global_code_array.size(); i++){
        fout << global_code_array[i] << endl;
    }
    fout.close();
}

string newLabel(){    
    static int label_no = 0;
    return "L" + to_string(label_no++);    
}

void makeSpaceForLocalVariables(Local_Symbol_Table* lst){
    int min = 0;
    for(auto it = lst->table.begin(); it != lst->table.end(); it++){
        if( (it->second).offset < min){
            min = (it->second).offset;
        }
    }
    genCode("\taddi(" + to_string(min) + ",esp); //making space for local variables");
}

void removeSpaceForLocalVariables(Local_Symbol_Table* lst){
    int min = 0;
    for(auto it = lst->table.begin(); it != lst->table.end(); it++){
        if( (it->second).offset < min){
            min = (it->second).offset;
        }
    }
    min = min * -1;    
    genCode("\taddi(" + to_string(min) + ",esp); //removing space for local variables");
}

void backpatch(list<int> jump_list, string label){
    for(auto it = jump_list.begin(); it != jump_list.end(); it++){
        string line = global_code_array[*it];
        vector<string> words;
        tokenize(line,words,"_");
        line = words[0] + label + words[1];
        global_code_array[*it] = line;
    }
}

list<int> merge(list<int> l1, list<int> l2){
    l1.insert(l1.end(),l2.begin(),l2.end());
    return l1;
}

list<int> makelist(int line_no){
    return list<int>(1, line_no);
}

bool compareFunction(ExpAst* exp1, ExpAst* exp2){
    return exp1->reg_requirement > exp2->reg_requirement;
}

string getRawOp(string op_name){
    int index = op_name.find("_INT");
    if(index != -1){
        return op_name.substr(0,index);
    } else{
        index = op_name.find("_FLOAT");
        if(index != -1){
            return op_name.substr(0,index);
        }
        return op_name;
    }

}

void clearCodeArray(){
    global_code_array = vector<string>();
}

void tokenize(const string& str, vector<string>& tokens, const string& delimiters){    
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}