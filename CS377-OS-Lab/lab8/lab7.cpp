#include <iostream>
#include <fstream>
#include <map>
#include <thread>
#include <vector>
#include <list>
#include <mutex>

using namespace std;
#define debug(X) { if(DEBUG) cout << X << endl;}
bool DEBUG = true;

void page_io_manager();
void free_frame_manager();


struct free_list_entry{
    int frame_number;
};
struct free_list_struct{
    list<free_list_entry> free_list;
}global_free_list;

struct io_table_entry{
    int page_no, pid, frame_no;
    bool is_page_in;
};
struct io_table_struct{
    list<io_table_entry> io_table;
};


struct PInfo{
    int size;
};

struct PageEntry{
    bool valid,dirty;//,readable,write;
    int frame_no;
};

map<int,PInfo> pdata;

static long timer;

struct PageTable{
    vector<PageEntry> page_entries; //indexed on page number
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

struct AccessTimeInfo{
	int frame_no, time, page_no, pid, dirty;
};

struct LRUData{list<AccessTimeInfo> frame_list;

    void insert(AccessTimeInfo access_time_info){
        frame_list.push_back(access_time_info);
    }

    AccessTimeInfo removeLast(){
        list<AccessTimeInfo>::iterator min = frame_list.begin();
        for(auto it = frame_list.begin(); it != frame_list.end(); it++){
            if(it->time < min->time){
                min = it;
            }
        }
        AccessTimeInfo access_time_info = *min;
        //debug(at.page_no);
        frame_list.erase(min);
        return access_time_info;
    }

    void update(bool is_access, int frame_no){
        for(auto it = frame_list.begin(); it != frame_list.end(); it++){
            if(it->frame_no == frame_no){
                if(!is_access){
                    it->dirty = true;
                }
                it->time = timer++;
            }
        }
    }
}lru_data;

void processT(int pid);
int mmu(bool is_access,int pid, int page_num);
int pageFaultHandler(int pid, int page_num);
string formatNumber(int num);
vector<string> stringTokenize(string line, char delimeter);
map<int, PageTable*> pagetable_data;
static int RAM_size = 0, l_thresh = 0; h_thresh = 0;

int main(){
    ifstream fin;
    string line;
    fin.open("init");
    vector<thread> threads;
    while(getline(fin,line)){
        vector<string> words = stringTokenize(line,' '); //TODO
        string command = words[0];
        if(command == "end"){
            break;
        } else{
            vector <string> parameters = stringTokenize(words[1],',');
            if(command == "create"){
                int pid = stoi(parameters[0]);
                PInfo pi;
                pi.size = stoi(parameters[1]);
                pdata[pid] = pi;
                threads.push_back(thread(processT,pid));
            } else if(command == "Memory_size"){
                RAM_size = parameters[0];
            } else if(command == "Lower_threshold"){
                l_thresh = parameters[0];
            } else if(command == "Upper_threshold"){
                h_thresh = parameters[0];
            }
        }
        int pid = stoi(line.substr(7,2));
    }
    for(auto it = threads.begin(); it!=threads.end();it++){
        it->join();
    }
}

void page_io_manager(){

}
void free_frame_manager(){

}
int pageFaultHandler(int pid, int page_num){
	PageTable* pt = pagetable_data[pid];
	AccessTimeInfo access_time_info;
	if(pt->isFull()) {
		//access_time_info = lru_data.removeLast();
        //debug(at.page_no);
        int frame_number_allocated = getFrameFromFreeList();
        cout<<"Frame allocated "<< frame_number_allocated<<endl;

        access_time_info.frame_no = frame_number_allocated;

		//int pn = access_time_info.page_no;
		//cout << "Removed page " << pn << " from frame " << pt->page_entries[pn].frame_no << endl;
		pt->page_entries[pn].valid = false;
		cout << "Loaded page " << page_num << " into frame " << pt->page_entries[pn].frame_no << endl;
		pt->page_entries[page_num].valid = true;
		pt->page_entries[page_num].frame_no = pt->page_entries[pn].frame_no;
	} else {
		cout << "Loaded page " << page_num << " into frame " << pt->first_free_frame << endl;
		access_time_info.frame_no = pt->first_free_frame;

		pt->page_entries[page_num].valid = true;
		pt->page_entries[page_num].frame_no = pt->first_free_frame;
		pt->first_free_frame++;
	}
	access_time_info.pid = pid;
	access_time_info.page_no = page_num;
	access_time_info.time = timer++;
	lru_data.insert(access_time_info);
    //cout << page_num << "-----------";
}

int mmu(bool is_access,int pid, int page_num){
    if(page_num >= pdata[pid].size){
        return -1; //mem protection
    }
    PageTable* pagetable = pagetable_data[pid];
    PageEntry this_page = pagetable->page_entries[page_num];

    if(!this_page.valid){
        pageFaultHandler(pid,page_num);
        return -2; //page fault occurred
    }
    lru_data.update(is_access, page_num);
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
        bool is_access = false;
        cout << pid << ": attempted to access/modify " << page_no << " " << word_no << endl;
        if(command == "access"){
            is_access = true;
            accessed++;
        }
        else if(command == "modify"){
            is_access = false;
            modified++;
        }
        int response = mmu(is_access,pid, stoi(page_no));
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

int getFrameFromFreeList(){
    free_list_entry selected_free_frame = global_free_list.free_list.front();
    global_free_list.free_list.erase(global_free_list.free_list.begin());
    return selected_free_frame.frame_number;
}

string formatNumber(int num){
    string st = to_string(num);
    for(int i=st.length(); i<4; i++){
        st = "0" + st;
    }
    return st;
}
