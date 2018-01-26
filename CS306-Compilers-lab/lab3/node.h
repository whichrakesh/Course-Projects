#ifndef Node_h_included
#define Node_h_included


#include <string>
#include <list>
#include <map>

using namespace std;

//#undef AbstractASTNode
class abstract_astnode
{
    public:
    virtual void print () = 0;
    //virtual std::string generate_code(const symbolTable&) = 0;
    //virtual basic_types getType() = 0;
    //virtual bool checkTypeofAST() = 0;
//    protected:
//    virtual void setType(basic_types) = 0;
//    private:
//    typeExp astnode_type;
};


class StmtAst: public abstract_astnode{

};

class ExpAst: public abstract_astnode{

};

class Empty: public StmtAst{
    public:
        Empty();
        void print();
};


class Block_stmt: public StmtAst{
    protected:
        list<StmtAst*> statement_list;

    public:
        Block_stmt(list<StmtAst*>* statement_list);
        void print();
};

class Ass: public StmtAst{
    protected:
        ExpAst *exp1, *exp2;

    public:
        Ass(ExpAst *exp1, ExpAst *exp2);
        void print();
};

class Return_class: public StmtAst{
    protected:
        ExpAst *exp;

    public:
        Return_class(ExpAst *exp);
        void print();
};

class If_class: public StmtAst{
    protected:
        ExpAst *exp;
        StmtAst *stmt1,*stmt2;

    public:
        If_class( ExpAst *exp, StmtAst *stmt1,StmtAst *stmt2);
        void print();
};

class While_class: public StmtAst{
    protected:
        ExpAst *exp;
        StmtAst *stmt;

    public:
        While_class(ExpAst *exp,StmtAst *stmt);
        void print();
};

class For_class: public StmtAst{
    protected:
        ExpAst *exp1,*exp2,*exp3;
        StmtAst *stmt;

    public:
        For_class(ExpAst *exp1,ExpAst *exp2,ExpAst *exp3, StmtAst *stmt);
        void print();
};

class Binary_op: public ExpAst{
    protected:
        string b_op;
        ExpAst *exp1,*exp2;

    public:
        Binary_op (string b_op, ExpAst* exp1, ExpAst* exp2);
        void print();
};

class Unary_op: public ExpAst{
    protected:
        string u_op;
        ExpAst *exp;

    public:
        Unary_op(string u_op, ExpAst *exp);
        void print();
};

class Fun_call: public ExpAst{
    protected:
        list<ExpAst*> expression_list;
    public:
        Fun_call(list<ExpAst*>* expression_list);
        void print();
};

class Float_const: public ExpAst{
    protected:
        float value;
    public:
        Float_const(float value);
        void print();
};

class Int_const: public ExpAst{
    protected:
        int value;
    public:
        Int_const(int value);
        void print();
};

class String_const: public ExpAst{
    protected:
        string value;
    public:
        String_const(string value);
        void print();
};

class Identifier: public ExpAst{
    protected:
        string value;
    public:
        Identifier(string value);
        void print();
};

class Arrayref: public ExpAst{
    protected:

};


class IdentifierArray: public Arrayref{
    protected:
        string value;
    public:
        IdentifierArray(string value);
        void print();
};

class Index: public Arrayref{
    protected:
        Arrayref *array_ref;
        ExpAst *exp;
    public:
        Index(Arrayref *array_ref, ExpAst *exp);
        void print();
};
#endif

