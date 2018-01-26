#include <iostream>
#include <vector>
#include <string>
#include "utils.h"
#include "trie.h"
using namespace std;
int main(){
	vector<entry> table;
	string dummy;
	cin>> dummy >> dummy;
	node* trie = new node();
	int entry_no = 0;
	while(true){
		string net_addr,net_mask;
		cin>>net_addr;
		if(net_addr == "default")
			break;
		cin>>net_mask;
		entry table_entry = {net_addr,net_mask};
		table.push_back(table_entry);
		string net_addr_binary = findNetAddrBinary(net_addr,net_mask);
		insertIntoTrie(trie,net_addr_binary,entry_no);
		//cout<< net_addr << " " << net_mask << endl;
		entry_no++;
	} 
	int ip_count;
	cin>>ip_count;
	while(ip_count--){
		string ip;
		cin>>ip;
		entry_no = findRoutingEntry(trie,ip);
		if(entry_no != -1){
			entry table_entry = table[entry_no];
			cout<< ip << " " << table_entry.net_addr << " " << table_entry.net_mask << endl;
		} else { 
			cout<< ip << " " << "default" << endl;
		}
	}
}
