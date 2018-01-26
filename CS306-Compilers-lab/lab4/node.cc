#include<iostream>
#include<vector>
#include "type_check.h"
#include "node.h"

using namespace std;

Type* AbstractASTNode::getType(){
	return type;
}

void AbstractASTNode::setType(Type* type){
	this->type = type;
}



void EmptyStmtAst::print(){
    cout<<"(Empty)";
}

BlockStmtAst::BlockStmtAst(list<StmtAst* >* statement_list){
    this->statement_list = *statement_list;
    //cout<<"rakesh"<<endl;
    for(auto it = statement_list->begin(); it != statement_list->end(); it++){
        if((*it)->getType()->tag != Type::Ok){
        	this->setType(new Type(Type::Error));
        	return;
        }        
    }
    this->setType(new Type(Type::Ok));
}

void BlockStmtAst::print(){
    cout<<"(Block\n";
    list<StmtAst*>::iterator it;
    for(it = statement_list.begin(); it != statement_list.end(); it++){
        (*it)->print();
        //cout << (*it)->getType()->toString();        
    }
    cout<<")\n";
}


AssignStmtAst::AssignStmtAst(ExpAst *exp1, ExpAst *exp2){
    this->exp1=exp1;
    this->exp2=exp2;
}
void AssignStmtAst::print(){
    cout << "( = ";
    exp1->print();
    cout << " "; 
    exp2->print();
    cout<<")\n";
}


ReturnStmtAst::ReturnStmtAst(ExpAst* exp){
    this->exp=exp;
}
void ReturnStmtAst::print(){
    cout<<"(return ";
    exp->print();
    cout<<")\n";
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


BinaryOpAst::BinaryOpAst(string b_op, ExpAst* exp1, ExpAst* exp2){	
    this->b_op = b_op;
    this->exp1 = exp1;
    this->exp2 = exp2;
    this->setType(resultType(exp1->getType(),exp2->getType()));
    if(this->b_op == "PLUS" || this->b_op == "MINUS" || this->b_op == "MULT" || this->b_op == "DIV")
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
}

void BinaryOpAst::print(){
    cout<<"( " << b_op << " ";
    exp1->print();
    cout << " ";
    exp2->print();
    cout<<" )";
}


UnaryOpAst::UnaryOpAst(string u_op, ExpAst* exp){
    this->u_op = u_op;
    this->exp = exp;
}
void UnaryOpAst::print(){
    cout<<"( "<< u_op;
    exp->print();
    cout << " )";
}

FunCallStmtAst::FunCallStmtAst(string name, list<ExpAst* >* expression_list){
    this->fun_name = name;
    this->expression_list = *expression_list;
}

void FunCallStmtAst::print(){
    list<ExpAst*>::iterator it;
    cout << "( FunCallStmtAst " << fun_name << " ";
    for(it = expression_list.begin(); it != expression_list.end(); it++){
        (*it)->print();
        cout<<" ";
    }
    cout << ")" << endl;
}

FunCallExpAst::FunCallExpAst(string name, list<ExpAst* >* expression_list){
    this->fun_name = name;
    this->expression_list = *expression_list;
    for(auto it = expression_list->begin(); it != expression_list->end(); it++){
        if((*it)->getType()->tag == Type::Error){
        	this->setType(new Type(Type::Error));
        	return;
        }
    }
    this->setType(new Type(Type::Ok));
}

void FunCallExpAst::print(){
    list<ExpAst*>::iterator it;
    cout << "( FunCallExpAst " << fun_name << " ";
    for(it = expression_list.begin(); it != expression_list.end(); it++){
        (*it)->print();
        cout<<" ";
    }
    cout << ")";
}


FloatConst::FloatConst(float value){
    this->value = value;
}
void FloatConst::print(){
    cout<<"( FloatConst "<<value<<" )";
}


IntConst::IntConst(int value){
    this->value = value;
}
void IntConst::print(){
    cout<<"( IntConst "<<value<<" )";
}


StringConst::StringConst(string value){
    this->value = value;
}
void StringConst::print(){
    cout<<"( StringLiteral \""<<value<<"\" )";
}


Identifier::Identifier(string value){
    this->value = value;
}
void Identifier::print(){
    cout<<"(Id: "<<value<<" )";
}

ToInt::ToInt(ExpAst *exp){
    this->exp = exp;
}

void ToInt::print(){
    cout << "(TO_INT ";
    exp->print();
    cout << " )";
}

ToFloat::ToFloat(ExpAst *exp){
    this->exp = exp;
}

void ToFloat::print(){
    cout << "(TO_FLOAT ";
    exp->print();
    cout << " )";
}

ArrayRefIdentifier::ArrayRefIdentifier(string value){
    this->value = value;
}

void ArrayRefIdentifier::print(){
    cout<<"( Identifier "<<value << " )";
}


Index::Index(ArrayRefAst* array_ref, ExpAst* exp){
    this->array_ref = array_ref;
    this->exp = exp;
}
void Index::print(){
    cout << "( ArrayIndex ";
    array_ref->print();
    cout << " ";
    exp->print();
    cout<<" )";
}




















