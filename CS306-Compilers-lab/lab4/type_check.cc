#include "type_check.h"

Type::Type() : tag(Ok) {
}
Type::Type(Kind kindval) : tag(kindval) {
}

Type::Type(Kind kindval, Basetype btype) : tag(kindval), basetype(btype) {
}

Type::Type(Kind kindval, int size, Type* type) : tag(kindval) {
	arraytype.size = size;
	arraytype.basetype = type;
}

string Type::toString()
{ 
  switch (tag) 
   {
   case Base: 
   	 switch (basetype) 
     {
       case Int: return "Int";
       break;
       case Float: return "Float";
     };
    break;
  case Error:
  	return "Error" ;
    break;
  case Ok:
  	return "Ok";
    break;
  case Array:
  		return "Array(" + to_string(arraytype.size) + "," + arraytype.basetype->toString()+ ")";
  }}

bool equal(Type* t1, Type* t2) {
	if (t1->tag == Type::Base && t2->tag == Type::Base)
		return (t1->basetype == t2->basetype);
	else if (t1->tag == Type::Array && t2->tag == Type::Array)
		return equal(t1->arraytype.basetype, t2->arraytype.basetype);
  return false;
}

bool compatible(Type* t1, Type* t2) {
	if(t1->tag == Type::Error || t1->tag == Type::Ok || t2->tag == Type::Error || t2->tag == Type::Ok){
		return false;
	}else if ((t1->basetype == Type::Int || t1->basetype == Type::Float)
			&& (t2->basetype == Type::Int || t2->basetype == Type::Float))
		return true;
	else
		return equal(t1, t2);
}

int Type::getSize(){
	if (tag == Base)
		return SIZE[basetype];
	else if (tag == Array){
		return arraytype.size * arraytype.basetype->getSize();
	}
		
}

Type* resultType(Type* t1, Type* t2){
	//cout << t1->toString() << t2->toString() << "rakesh" << endl;	
	if (t1->tag == Type::Error ||  t2->tag == Type::Error)   // error propagation	      
	    return new Type(Type::Error);
	else if (t1->tag == Type::Array || 
	   t2->tag == Type::Array)
	    return new Type(Type::Error);
	else if (t1->basetype == Type::Int && 
	           t2->basetype == Type::Int)
	    return new Type(Type::Base, Type::Int);
	else 
		return new Type(Type::Base, Type::Float);
}

void Local_Symbol_Table::insert(string lex, Type* type, int offset){
    Symbol_Desc s;
    s.type = type;
    s.offset = offset;
    table[lex] = s;
}
void Local_Symbol_Table::dump(){
    ofstream fout;
    fout.open("symboltable.txt",ios::app);
    fout << "\nFunction: " << func_name << endl;
    fout<<"----------------------------------------------------------------------" << endl;
    fout<<"Symbol\tType\tOffset\n";
    fout<<"----------------------------------------------------------------------" << endl;
    for(auto it = table.begin(); it != table.end(); it++){
        fout << it->first << "\t"<< (it->second).type->toString() << "\t" << (it->second).offset << endl;
    }
    cout << endl<< endl;
    fout.close();
}
void Global_Symbol_Table::insert(string func_name, Type* return_type, Local_Symbol_Table* s){
	Function_Desc fd;
	fd.return_type = return_type;
	fd.lst = s;
    table[func_name] = fd;
}

void Local_Symbol_Table::addParameter(string p){
  parameters.push_back(p);
}

Type* Local_Symbol_Table::getParameterType(int i){
  if(i < parameters.size()){
    return table[parameters[i]].type;
  }
  return new Type(Type::Error);
}
