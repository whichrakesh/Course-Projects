#ifndef _UTILS_H
#define _UTILS_H
#include <string>
using namespace std;
struct entry{
	string net_addr;
	string net_mask;
};

string findNetAddrBinary(string net_addr, string net_mask);
string addrToBinary(string addr);
string toBinary(int num);

#endif
