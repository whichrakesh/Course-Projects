#include<iostream>
#include<vector>
#include "node.h"

using namespace std;

Empty::Empty(){
}
void Empty::print(){
    cout<<"(Empty)";
}

Block_stmt::Block_stmt(list<StmtAst* >* statement_list){
    this->statement_list = *statement_list;
}
void Block_stmt::print(){
    cout<<"(Block\n";
    list<StmtAst*>::iterator it;
    for(it = statement_list.begin(); it != statement_list.end(); it++){
        (*it)->print();        
    }
    cout<<")\n";
}


Ass::Ass(ExpAst *exp1, ExpAst *exp2){
    this->exp1=exp1;
    this->exp2=exp2;
}
void Ass::print(){
    cout << "( = ";
    exp1->print();
    cout << " "; 
    exp2->print();
    cout<<")\n";
}


Return_class::Return_class(ExpAst* exp){
    this->exp=exp;
}
void Return_class::print(){
    cout<<"(return ";
    exp->print();
    cout<<")\n";
}


If_class::If_class(ExpAst* exp, StmtAst* stmt1, StmtAst* stmt2){
    this->exp = exp;
    this->stmt1 = stmt1;
    this->stmt2 = stmt2;
}
void If_class::print(){
    cout<<"( if ";
    exp->print();
    cout << " "; 
    stmt1->print();
    cout << " "; 
    stmt2->print();
    cout<<" )\n";
}


While_class::While_class(ExpAst* exp, StmtAst* stmt){
    this->exp = exp;
    this->stmt = stmt;
}
void While_class::print(){
    cout<<"( while ";
    exp->print();
    cout << " "; 
    stmt->print();
    cout<<" )\n";
}


For_class::For_class(ExpAst* exp1, ExpAst* exp2, ExpAst* exp3, StmtAst* stmt){
    this->exp1 = exp1;
    this->exp2 = exp2;
    this->exp3 = exp3;
    this->stmt = stmt;
}
void For_class::print(){
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


Binary_op::Binary_op(string b_op, ExpAst* exp1, ExpAst* exp2){
    this->b_op = b_op;
    this->exp1 = exp1;
    this->exp2 = exp2;
}
void Binary_op::print(){
    cout<<"( " << b_op;
    cout << " ";    
    exp1->print();
    cout << " ";
    exp2->print();
    cout<<" )";
}


Unary_op::Unary_op(string u_op, ExpAst* exp){
    this->u_op = u_op;
    this->exp = exp;
}
void Unary_op::print(){
    cout<<"( "<< u_op;
    exp->print();
    cout << " )";
}


Fun_call::Fun_call(list<ExpAst* >* expression_list){
    this->expression_list = *expression_list;
}

void Fun_call::print(){
    list<ExpAst*>::iterator it;
    cout << "( Fun_call";
    for(it = expression_list.begin(); it != expression_list.end(); it++){
        (*it)->print();
        cout<<" ";
    }
    cout << ")";
}


Float_const::Float_const(float value){
    this->value = value;
}
void Float_const::print(){
    cout<<"( FloatConst "<<value<<" )";
}


Int_const::Int_const(int value){
    this->value = value;
}
void Int_const::print(){
    cout<<"( IntConst "<<value<<" )";
}


String_const::String_const(string value){
    this->value = value;
}
void String_const::print(){
    cout<<"( StringLiteral \""<<value<<"\" )";
}


Identifier::Identifier(string value){
    this->value = value;
}
void Identifier::print(){
    cout<<"(Id: "<<value<<" )";
}



IdentifierArray::IdentifierArray(string value){
    this->value = value;
}
void IdentifierArray::print(){
    cout<<"( ArrayString "<<value << " )";
}


Index::Index(Arrayref* array_ref, ExpAst* exp){
    this->array_ref = array_ref;
    this->exp = exp;
}
void Index::print(){
    cout << "( Index ";
    array_ref->print();
    cout << " ";
    exp->print();
    cout<<" )";
}




















