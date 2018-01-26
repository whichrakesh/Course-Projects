#include <iostream>
#include <fstream>
#include <map>
#include <thread>
#include <vector>
#include <list>
using namespace std;
#define debug(X) { if(DEBUG) cout << X << endl;}
bool DEBUG = true;

struct PInfo{
    int size;
    int frame_start;
    int frame_end;
};

struct PageEntry{
    bool valid;//,dirty,readable,write;
    int frame_no;
};

map<int,PInfo> pdata;
static long timer;

struct AccessTimeInfo{
    int page_no;
    int time;    
};

struct LRUData{
    list<AccessTimeInfo> page_list;
    void insert(AccessTimeInfo at){
        page_list.push_back(at);
    }
    AccessTimeInfo removeLast(){
        list<AccessTimeInfo>::iterator min = page_list.begin();
        for(auto it = page_list.begin(); it != page_list.end(); it++){
            if(it->time < min->time){
                min = it;
            }
        }
        AccessTimeInfo at = *min;
        //debug(at.page_no);
        page_list.erase(min);
        return at;
    }
    void update( int page_no){
        for(auto it = page_list.begin(); it != page_list.end(); it++){
            if(it->page_no == page_no)
            {
                it->time = timer++;
            }
        }
    }
};

struct PageTable{
    vector<PageEntry> page_entries;
    LRUData lru_data;
    int size;
    int first_free_frame;
    int last_frame;
    PageTable(int pid){
        this->size = pdata[pid].size;
        first_free_frame = pdata[pid].frame_start;
        last_frame = pdata[pid].frame_end;
        page_entries.resize(size);
        for(int i = 0; i < size; i++){
            page_entries[i].valid = false;
        }
    }
    bool isFull(){
    	return first_free_frame > last_frame;
    }
};

void processT(int pid);
int mmu(int pid, int page_num);
int pageFaultHandler(int pid, int page_num);
string formatNumber(int num);
map<int, PageTable*> pagetable_data;

int main(){
    ifstream fin;
    string line;
    fin.open("init");
    vector<thread> threads;
    while(getline(fin,line)){
        if(line.substr(0,3) == "end"){
            break;
        }
        int pid = stoi(line.substr(7,2));
        PInfo pi;
        pi.size = stoi(line.substr(10,3));
        pi.frame_start = stoi(line.substr(14,3));
        pi.frame_end = stoi(line.substr(18,3));
        pdata[pid] = pi;
        //debug(pid);
        //thread first(processT,pid);
        threads.push_back(thread(processT,pid));        
    }
    for(auto it = threads.begin(); it!=threads.end();it++){
        it->join();
    }
}


int pageFaultHandler(int pid, int page_num){
	PageTable* pt = pagetable_data[pid];
	AccessTimeInfo at;
	if(pt->isFull()) {
		at = pt->lru_data.removeLast();
        //debug(at.page_no);
		int pn = at.page_no;
		cout << "Removed page " << pn << " from frame " << pt->page_entries[pn].frame_no << endl;
		pt->page_entries[pn].valid = false;
		cout << "Loaded page " << page_num << " into frame " << pt->page_entries[pn].frame_no << endl;
		pt->page_entries[page_num].valid = true;
		pt->page_entries[page_num].frame_no = pt->page_entries[pn].frame_no;		
	} else {
		cout << "Loaded page " << page_num << " into frame " << pt->first_free_frame << endl;
		pt->page_entries[page_num].valid = true;
		pt->page_entries[page_num].frame_no = pt->first_free_frame;
		pt->first_free_frame++;
	}
	at.page_no = page_num;
	at.time = timer++;
	pt->lru_data.insert(at);
    //cout << page_num << "-----------";
}

int mmu(int pid, int page_num){
    if(page_num >= pdata[pid].size){        
        return -1; //mem protection
    }
    PageTable* pagetable = pagetable_data[pid];
    PageEntry this_page = pagetable->page_entries[page_num];

    if(!this_page.valid){
        pageFaultHandler(pid,page_num);
        return -2; //page fault occurred
    }     
    pagetable->lru_data.update(page_num);   
    return this_page.frame_no;
}


void processT(int pid){
    ifstream infile("s"+to_string(pid));
    string command, page_no, word_no;
    int accessed = 0, modified = 0, pagefaults = 0;
    PageTable* pagetable = new PageTable(pid);
    pagetable_data[pid] = pagetable;   
    while (infile >> command){
        if(command == "end") {
        	cout << "Number of access operations : " << formatNumber(accessed) << endl;
        	cout << "Number of modify operations : " << formatNumber(modified) << endl;
        	cout << "Number of page faults : " << formatNumber(pagefaults) << endl;
            return ;
        }
        infile >> page_no >> word_no;
        bool ins = false;
        cout << pid << ": attempted to access/modify " << page_no << " " << word_no << endl;
        if(command == "access"){
            accessed++;
        }
        else if(command == "modify"){
            modified++;
        }
        int response = mmu(pid, stoi(page_no));   
        if(response == -1){
            cout << "Reported a memory protection violation" << endl;
        }else if(response == -2){
            cout << "Reported a page fault" << endl;
            pagefaults++;
        }else{
            cout << "Accessed page frame number "<< response << endl;
        }
    }
}

string formatNumber(int num){
    string st = to_string(num);
    for(int i=st.length(); i<4; i++){
        st = "0" + st;
    }
    return st;
}
