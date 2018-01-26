#include <iostream>
#include <vector>
#include <string>
#include "trie.h"
#include "utils.h"
using namespace std;
void insertIntoTrie(node* trie, string net_addr, int entry_no){ // inserts the ip network address to the trie
	for(unsigned int i=0; i<net_addr.length(); i++){
		if(net_addr[i] == '0'){
			if(trie->child[0] != NULL){
				trie = trie->child[0];
			}
			else{
				node* child = new node();
				trie->child[0] = child;
				trie = child;
			}
		} else 
			if(net_addr[i] == '1'){
				if(trie->child[1] != NULL){
					trie = trie->child[1];
				}
				else{
					node* child = new node();
					trie->child[1] = child;
					trie = child;
				}
			}
		else
			break;
	}
	trie->entry_no = entry_no;
}

int findRoutingEntry(node* trie, string ip){ // finds the routing entry corresponding to the given ip in the forwarding table
	string binary_ip =	addrToBinary(ip);
	int entry_no = -1;
	for(unsigned int i = 0; i < binary_ip.length(); i++){
		if(binary_ip[i] == '0'){
			if(trie->child[0] != NULL){
				trie = trie->child[0];
			}
			else {
				break;
			}
		}
		else 
		if(binary_ip[i] == '1'){
			if(trie->child[1] != NULL){
				trie = trie->child[1];
			}
			else {
				break;
			}
		}
		else break;
		//cout<<"done";
		if(trie->entry_no != -1){
			entry_no = trie->entry_no;
		}		
	}
	return entry_no;
}
