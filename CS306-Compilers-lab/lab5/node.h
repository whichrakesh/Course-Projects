#ifndef Node_h_included
#define Node_h_included

#include <fstream>
#include <string>
#include <list>
#include <map>
#include <stack>
#include "type_check.h"
using namespace std;


/****************************************************************************************/
//#undef AbstractASTNode
class RegStack{
    protected:
        stack<string> r_stack;
        list<string> used_regs;
    public:
        RegStack();
        void push(string reg);
        void swap();
        string top();
        string pop();
        int size();
        list<string> getUsedRegisters();
};

class AbstractASTNode
{
    public:
	    virtual void print () = 0;
	    virtual void generate_code(Local_Symbol_Table*) = 0;
	    Type* getType();
	   	// virtual bool checkTypeofAST() = 0;
	    void setType(Type*);
    protected:    	
    private:
	    Type* type;
};


class StmtAst: public AbstractASTNode{
    public:
        list<int> nextlist;
};

class ExpAst: public AbstractASTNode{
	public:
        list<int> truelist,falselist;
        int reg_requirement = 0;
        enum ExpType{
            Const, Variable
        };
        ExpType exp_type = Variable;
        bool left_to_right = false;
        union{
            float fvalue;
            int ivalue;
        };
        virtual list<ExpAst*> recomputeExpTree(string op)=0;
};


class ArrayRefAst: public ExpAst{
    public:
        int offset;
        enum ToPass{
            Value, Address, ValueAddress
        };
        ToPass to_pass = Value;
};


class EmptyStmtAst: public StmtAst{
    public:
        void print();
        void generate_code(Local_Symbol_Table*);
};


class BlockStmtAst: public StmtAst{
    protected:
        list<StmtAst*> statement_list;

    public:
        BlockStmtAst(list<StmtAst*>* statement_list);
        void print();
        void generate_code(Local_Symbol_Table*);
};

class AssignStmtAst: public StmtAst{
    protected:
        ArrayRefAst *exp1;
        ExpAst *exp2;
    public:
        AssignStmtAst(ArrayRefAst *exp1, ExpAst *exp2);
        void generate_code(Local_Symbol_Table*);
        void print();
};

class ReturnStmtAst: public StmtAst{
    protected:
        ExpAst *exp;

    public:
        ReturnStmtAst(ExpAst *exp);
        void generate_code(Local_Symbol_Table*);
        void print();
};

class IfStmtAst: public StmtAst{
    protected:
        ExpAst *exp;
        StmtAst *stmt1,*stmt2;

    public:
        IfStmtAst( ExpAst *exp, StmtAst *stmt1,StmtAst *stmt2);
        void print();
        void generate_code(Local_Symbol_Table*);
};

class WhileStmtAst: public StmtAst{
    protected:
        ExpAst *exp;
        StmtAst *stmt;

    public:
        WhileStmtAst(ExpAst *exp,StmtAst *stmt);
        void print();
        void generate_code(Local_Symbol_Table*);
};

class ForStmtAst: public StmtAst{
    protected:
        ExpAst *exp1,*exp2,*exp3;
        StmtAst *stmt;

    public:
        ForStmtAst(ExpAst *exp1,ExpAst *exp2,ExpAst *exp3, StmtAst *stmt);
        void print();
        void generate_code(Local_Symbol_Table*);
};

class FunCallStmtAst: public StmtAst{
    protected:
        string func_name;
        list<ExpAst*> expression_list;
    public:
        FunCallStmtAst(string name, list<ExpAst*>* expression_list);
        void print();
        void generate_code(Local_Symbol_Table*);
};

class BinaryOpAst: public ExpAst{
    protected:
        string b_op;
        ExpAst *exp1,*exp2;        
    public:
        BinaryOpAst (string b_op, ExpAst* exp1, ExpAst* exp2);
        void print();
        void generate_code(Local_Symbol_Table*);
        list<ExpAst*> recomputeExpTree(string op);
};

class UnaryOpAst: public ExpAst{
    protected:
        string u_op;
        ExpAst *exp;

    public:
        UnaryOpAst(string u_op, ExpAst *exp);
        void print();
        void generate_code(Local_Symbol_Table*);
        list<ExpAst*> recomputeExpTree(string op);
};

class FunCallExpAst: public ExpAst{
    protected:
        string func_name;
        list<ExpAst*> expression_list;
    public:
        FunCallExpAst(string name, list<ExpAst*>* expression_list);
        void print();
        void generate_code(Local_Symbol_Table*);
        list<ExpAst*> recomputeExpTree(string op);
};

class FloatConst: public ExpAst{
    protected:
        float value;
    public:
        FloatConst(float value);
        void print();
        void generate_code(Local_Symbol_Table*);
        list<ExpAst*> recomputeExpTree(string op);
};

class IntConst: public ExpAst{
    protected:
        int value;
    public:
        IntConst(int value);
        void print();
        void generate_code(Local_Symbol_Table*);
        list<ExpAst*> recomputeExpTree(string op);
};

class StringConst: public ExpAst{
    protected:
        string value;
    public:
        StringConst(string value);
        void print();
        void generate_code(Local_Symbol_Table*);
        list<ExpAst*> recomputeExpTree(string op);
};

class Identifier: public ExpAst{
    protected:
        string value;
    public:
        Identifier(string value);
        void print();
        void generate_code(Local_Symbol_Table*);
        list<ExpAst*> recomputeExpTree(string op);
};

class ToInt: public ExpAst{
    protected:
        ExpAst* exp;
    public:
        ToInt(ExpAst* exp);
        void print();  
        void generate_code(Local_Symbol_Table*);
        list<ExpAst*> recomputeExpTree(string op);
};

class ToFloat: public ExpAst{
    protected:
        ExpAst *exp;
    public:
        ToFloat(ExpAst* exp);
        void print();
        void generate_code(Local_Symbol_Table*);
        list<ExpAst*> recomputeExpTree(string op);
};


class ArrayRefIdentifier: public ArrayRefAst{
    protected:
        string value;
    public:
        ArrayRefIdentifier(string value);
        void print();
        void generate_code(Local_Symbol_Table*);
        list<ExpAst*> recomputeExpTree(string op);
};

class Index: public ArrayRefAst{
    protected:
        ArrayRefAst *array_ref;
        ExpAst *exp;
    public:
        Index(ArrayRefAst *array_ref, ExpAst *exp);
        void print();
        void generate_code(Local_Symbol_Table*);
        list<ExpAst*> recomputeExpTree(string op);
};

void genCode(string s);
void printCode();
string newLabel();
void makeSpaceForLocalVariables(Local_Symbol_Table*);
void removeSpaceForLocalVariables(Local_Symbol_Table* lst);
void backpatch(list<int> jump_list, string label);
list<int> merge(list<int> l1, list<int> l2);
list<int> makelist(int line_no);
void clearCodeArray();
void tokenize(const string& str, vector<string>& tokens, const string& delimiters);
bool compareFunction(ExpAst* exp1, ExpAst* exp2);
string getRawOp(string op_name);
#endif

