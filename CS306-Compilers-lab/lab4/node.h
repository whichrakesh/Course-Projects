#ifndef Node_h_included
#define Node_h_included

#include <fstream>
#include <string>
#include <list>
#include <map>
#include "type_check.h"
using namespace std;


/****************************************************************************************/
//#undef AbstractASTNode
class AbstractASTNode
{
    public:
	    virtual void print () = 0;
	    //virtual std::string generate_code(const symbolTable&) = 0;
	    Type* getType();
//	   	virtual bool checkTypeofAST() = 0;
	    void setType(Type*);
    protected:    	
    private:
	    Type* type;
};


class StmtAst: public AbstractASTNode{

};

class ExpAst: public AbstractASTNode{
	public:
};

class EmptyStmtAst: public StmtAst{
    public:
        void print();
};


class BlockStmtAst: public StmtAst{
    protected:
        list<StmtAst*> statement_list;

    public:
        BlockStmtAst(list<StmtAst*>* statement_list);
        void print();
};

class AssignStmtAst: public StmtAst{
    protected:
        ExpAst *exp1, *exp2;

    public:
        AssignStmtAst(ExpAst *exp1, ExpAst *exp2);
        void print();
};

class ReturnStmtAst: public StmtAst{
    protected:
        ExpAst *exp;

    public:
        ReturnStmtAst(ExpAst *exp);
        void print();
};

class IfStmtAst: public StmtAst{
    protected:
        ExpAst *exp;
        StmtAst *stmt1,*stmt2;

    public:
        IfStmtAst( ExpAst *exp, StmtAst *stmt1,StmtAst *stmt2);
        void print();
};

class WhileStmtAst: public StmtAst{
    protected:
        ExpAst *exp;
        StmtAst *stmt;

    public:
        WhileStmtAst(ExpAst *exp,StmtAst *stmt);
        void print();
};

class ForStmtAst: public StmtAst{
    protected:
        ExpAst *exp1,*exp2,*exp3;
        StmtAst *stmt;

    public:
        ForStmtAst(ExpAst *exp1,ExpAst *exp2,ExpAst *exp3, StmtAst *stmt);
        void print();
};

class FunCallStmtAst: public StmtAst{
    protected:
        string fun_name;
        list<ExpAst*> expression_list;
    public:
        FunCallStmtAst(string name, list<ExpAst*>* expression_list);
        void print();
};

class BinaryOpAst: public ExpAst{
    protected:
        string b_op;
        ExpAst *exp1,*exp2;

    public:
        BinaryOpAst (string b_op, ExpAst* exp1, ExpAst* exp2);
        void print();
};

class UnaryOpAst: public ExpAst{
    protected:
        string u_op;
        ExpAst *exp;

    public:
        UnaryOpAst(string u_op, ExpAst *exp);
        void print();
};

class FunCallExpAst: public ExpAst{
    protected:
        string fun_name;
        list<ExpAst*> expression_list;
    public:
        FunCallExpAst(string name, list<ExpAst*>* expression_list);
        void print();
};

class FloatConst: public ExpAst{
    protected:
        float value;
    public:
        FloatConst(float value);
        void print();
};

class IntConst: public ExpAst{
    protected:
        int value;
    public:
        IntConst(int value);
        void print();
};

class StringConst: public ExpAst{
    protected:
        string value;
    public:
        StringConst(string value);
        void print();
};

class Identifier: public ExpAst{
    protected:
        string value;
    public:
        Identifier(string value);
        void print();
};

class ToInt: public ExpAst{
    protected:
        ExpAst* exp;
    public:
        ToInt(ExpAst* exp);
        void print();  
};

class ToFloat: public ExpAst{
    protected:
        ExpAst *exp;
    public:
        ToFloat(ExpAst* exp);
        void print();
};

class ArrayRefAst: public ExpAst{
    protected:

};


class ArrayRefIdentifier: public ArrayRefAst{
    protected:
        string value;
    public:
        ArrayRefIdentifier(string value);
        void print();
};

class Index: public ArrayRefAst{
    protected:
        ArrayRefAst *array_ref;
        ExpAst *exp;
    public:
        Index(ArrayRefAst *array_ref, ExpAst *exp);
        void print();
};

#endif

