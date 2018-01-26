#ifndef Typecheck_h_included
#define Typecheck_h_included
#include <map>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

static int SIZE[] = {4,8,0};
class Type {
    public:
     enum Kind {
         Base, Array, Error, Ok
     };     
     enum Basetype {
         Int, Float, Void
     };     
     Kind tag;
     struct Arraytype{
        Type* basetype;
        int size;   
     };
     union {
         Basetype basetype;
         Arraytype arraytype;
     };
     Type();    // Default
     Type(Kind); // Error, Ok
     Type(Kind, Basetype); //Int, Float
     Type(Kind kindval, int size, Type* type);   // Array
     string toString();
     int getSize();
};

struct Symbol_Desc{
    Type* type;
    int offset;
};
struct Local_Symbol_Table{
    string func_name;
    map<string,Symbol_Desc> table;
    vector<string> parameters;
    void insert(string lex, Type* type, int offset);
    Type* getParameterType(int i);
    void addParameter(string p); 
    void dump();
};
struct Function_Desc{
	Type* return_type;
	Local_Symbol_Table* lst;
};
struct Global_Symbol_Table{
    map<string,Function_Desc> table;
    void insert(string func_name, Type* return_type, Local_Symbol_Table* s);    
};

bool equal(Type* t1, Type* t2);
bool compatible(Type* t1, Type* t2);
Type* resultType(Type* t1, Type* t2);
#endif