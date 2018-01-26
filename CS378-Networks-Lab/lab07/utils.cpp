#include <iostream>
#include <string>
#include "utils.h"
using namespace std;

string findNetAddrBinary(string net_addr, string net_mask){ // finds subnet adresses in binary
	string binary_net = addrToBinary(net_addr);
	string binary_mask = addrToBinary(net_mask);
	int mask_length = 0;
	for(unsigned int i=0; i < binary_mask.length(); i++){
		if(binary_mask[i] == '1')
			mask_length++;
		else
			break;
	}
	string binary_net_addr = binary_net.substr(0,mask_length);
	//cout<<binary_net << " : " << binary_mask << " : " << binary_net_addr << endl;
	return binary_net_addr;
}	

string addrToBinary(string ip){ // converts an ip address to binary
	int num = 0;
	string binary_ip;
	for(unsigned int i=0; i < ip.length(); i++){
		if(ip[i]=='.'){
			binary_ip += toBinary(num);
			num = 0;
		}
		else{
			num *= 10;
			num +=	ip[i] - '0';
		}
	}	
	binary_ip += toBinary(num);
	//cout<<ip << " : " << binary_ip << endl;
	return binary_ip;
}
string toBinary( int num){ // converts a number to binary
	string binary = "00000000";
	if(num == 0)
		return binary;
	int index = 0;
	while(num != 0){
		binary[7 - index] = '0' + num % 2;
		num /= 2;
		index++;
	}
	return binary;
}



