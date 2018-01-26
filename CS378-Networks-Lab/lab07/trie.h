#ifndef _TRIE_H
#define _TRIE_H
#include <string>
using namespace std;
struct node{
	int entry_no;
	node* child[2];
	node(){
		entry_no = -1;
		child[0] = NULL;
		child[1] = NULL;
	}
};
void insertIntoTrie(node* trie, string net_addr, int entry_no);
int findRoutingEntry(node* trie, string ip);
#endif
