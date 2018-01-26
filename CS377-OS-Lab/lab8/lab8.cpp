#include <iostream>
#include <fstream>
#include <mutex>
#include <map>
#include <thread>
#include <vector>
#include <condition_variable>
#include <set>
#include <string>
#include <algorithm>
#include <iterator>
#include <list>
#include <chrono>
using namespace std;
#define debug(X) { unique_lock<mutex> locker(mutex_cout); if(DEBUG) cout << X<<endl; locker.unlock();}
bool DEBUG = false;
int RAM_size, l_thresh, h_thresh;
static long timer;

mutex mutex_io_table, mutex_free_list, mutex_lru_table, mutex_pt,mutex_cout;
condition_variable cond_less_than_thres, cond_more_than_thresh;
condition_variable cond_page_i_completed, cond_page_o_completed, cond_io_task_arrived;

void tokenize(const string& str,vector<string>& tokens,const string& delimiters);

struct PageTable;
struct PInfo{
    int process_size;

};

struct IOTableEntry{
    int page_no, pid, frame_no;
    bool page_in, page_out;
    bool is_access;
    IOTableEntry(int page_no,int frame_no, int pid, bool page_in, bool page_out, bool is_access){
        this->page_in = page_in;
        this->page_out = page_out;
        this->frame_no = frame_no;
        this->pid = pid;
        this->page_no = page_no;
        this->is_access = is_access;
    }

};

struct IOTableInfo{
    list<IOTableEntry> io_table;

    int addIOTask(int page_no,int frame_no, int pid, bool page_in, bool page_out, bool is_access){
        IOTableEntry new_io(page_no, frame_no, pid, page_in, page_out,is_access);
        unique_lock<mutex> locker(mutex_io_table);
        debug("new io added page:"<<page_no<<" frame:"<<frame_no<<" pid:"<<pid<<" page_in:"<<page_in<<" page_out:"<<page_out<<endl);
        io_table.push_back(new_io);
        locker.unlock();
        cond_io_task_arrived.notify_all();
        debug("io table notified io manager\n");
    }
}global_io_table;

struct FreeListInfo{
    list<int> free_list;

    int getFreePage(int pid){
        int ret = free_list.front();
        free_list.erase(free_list.begin());
        debug("free page:"<<ret<<" returned for allocation to pid:"<<pid<<endl);
        return ret;
    }
    void addFreeFrame(int frame_number){
        debug("frame:"<<frame_number<<" added to the freelist\n");
        free_list.push_back(frame_number);
    }
    void initialize(){
    debug("intializing freelist...\n");
        for(int i = 0; i < RAM_size ; i++){
            debug("initialising frame:"<<i<<" added to free list\n");
            free_list.push_back(i);
        }
    }
}global_free_list;

map<int,PInfo> pdata;

struct PageEntry{
    bool valid,dirty;//,readable,write;
    int frame_no;
    PageEntry(){
        valid = false;
    }
    PageEntry(bool v, bool d, int f){
        valid = v;
        dirty = d;
        frame_no = f;
    }
};


map<int, PageTable*> global_page_table_list;


struct PageTable{
    vector<PageEntry> page_entries;
    int size_;
    int pid;

    PageTable(int pid){
        this->pid = pid;
        global_page_table_list[pid] = this;
        size_ = pdata[pid].process_size;
        page_entries.resize(size_);
    }

    void addPageEntry(PageEntry& page_entry, int& page_no){
        page_entries[page_no] = page_entry;
        debug("pageentry added for page:"<<page_no<<" on frame no:"<<page_entry.frame_no<<" with pid:"<<pid<<endl);
    }

    void clear_entry(int& page_no){
        debug("pageentry deleted for page:"<<page_no<<" and frame no."<<page_entries[page_no].frame_no<<" with pid:"<<pid<< " deleted.\n");
        page_entries[page_no].valid = false;
        page_entries[page_no].dirty = false;
        page_entries[page_no].frame_no = -1;

    }

    bool isEmpty(int& page_number){
        bool to_return = !page_entries[page_number].valid;
        debug("is empty pageentry page:"<<page_number<<"and with pid:"<<pid<<endl);
    }
    void print_pt_table(){

        for(int i = 0; i < size_; i++){
            PageEntry temp_p = page_entries[i];
            cout<<i<<"\t |"<<temp_p.frame_no<<"\t |"<<temp_p.valid<<"\t |"<<temp_p.dirty<<"\n";
        }
    }
};

struct LRU_entry{

    int pid;
    int page_number;
	int frame_no;
	int time;
	bool dirty;
	LRU_entry(){}
	LRU_entry(int frame_no, int time, bool dirty){
        pid = -1;
        page_number = -1;
        this->dirty = dirty;
        this->frame_no = frame_no;
        this->time = time;
	}
	LRU_entry(int frame_no, int time,int page_number,int pid, bool dirty){
        this->dirty = dirty;
        this->frame_no = frame_no;
        this->time = time;
        this->page_number= page_number;
        this->pid = pid;
	}
};


struct LRU_entry_comp{
  bool operator() (const LRU_entry& lhs,const LRU_entry& rhs){
    bool to_return;
    if(lhs.time<rhs.time) return true;
    else if(lhs.time>rhs.time) return false;
    else{
        return lhs.frame_no<rhs.frame_no;
    }
  }
};

struct LRUData{
    set<LRU_entry,LRU_entry_comp> LRU_table;
    map<int, set<LRU_entry>::iterator > backptr_pa_2_lru_iterator;

//    void initialise(){
//        for(int i = 0; i < RAM_size; i++){
//            LRU_entry temp(i,0,false);
//            pair<set<LRU_entry>::iterator,bool> ret = LRU_table.insert(temp);
//            backptr_pa_2_lru_iterator[i] = ret.first;
//        }
//    }
    void print(){
        debug("LRU Table:");
        for(auto it = LRU_table.begin(); it != LRU_table.end(); it++){
            debug("frame:" <<it->frame_no <<" page:" << it->page_number << " dirty:" << it->dirty);
        }
    }
    void insertLRUEntry(int& frame_no, LRU_entry& lru_entry){
        unique_lock<mutex> lru_locker(mutex_lru_table);
        debug("lruEntry added frame:"<<frame_no<<" pid:"<<lru_entry.pid<<" page_number:"<<lru_entry.page_number<< "`" << lru_entry.dirty << endl);
        pair<set<LRU_entry>::iterator,bool> ret = LRU_table.insert(lru_entry);
        backptr_pa_2_lru_iterator[frame_no] = ret.first;
        lru_locker.unlock();
        print();
        if(RAM_size - LRU_table.size() < l_thresh){
            debug("free list size less than lower thresh\n");
            cond_less_than_thres.notify_all();
        }
    }

    int update(int pid, int frame_num, bool is_access_instruction){
        set<LRU_entry>::iterator lru_iterator = backptr_pa_2_lru_iterator[frame_num];
        LRU_entry temp = *lru_iterator;
        unique_lock<mutex> lru_locker(mutex_lru_table);
        LRU_table.erase(lru_iterator);
        backptr_pa_2_lru_iterator.erase(frame_num);
        temp.time = timer++;
        if(!is_access_instruction){
            debug("this is write instruction modifiing dirty bit of lru data. frame no:"<<frame_num<<" pid:"<<pid<<" page no:"<<temp.page_number<<endl);
            (global_page_table_list[pid]->page_entries[frame_num]).dirty = true;
            temp.dirty = true;
            backptr_pa_2_lru_iterator[frame_num] = (LRU_table.insert(temp)).first;

        }
        else{
            debug("this is read instruction. frame no:"<<frame_num<<" pid:"<<pid<<" page no:"<<temp.page_number<<endl);
            backptr_pa_2_lru_iterator[frame_num] = (LRU_table.insert(temp)).first;
        }
        lru_locker.unlock();
        return 1;
    }

    void eraseLRUEntry(int frame_no){
        unique_lock<mutex> lru_locker(mutex_lru_table);
        auto lru_iterator = backptr_pa_2_lru_iterator[frame_no];
        backptr_pa_2_lru_iterator.erase(frame_no);
        debug(LRU_table.size());
        print();
        LRU_table.erase(lru_iterator);
        debug(LRU_table.size());
        print();
        debug("lru entry for frame:"<<frame_no<<"deleted");
        lru_locker.unlock();
    }

    LRU_entry getLRUEntryTORemove(){
        unique_lock<mutex> lru_locker(mutex_lru_table);
        LRU_entry lru_entry = *(LRU_table.begin());
        lru_locker.unlock();
        debug("first lru entry to return frame:"<<lru_entry.frame_no<<" pid:"<<lru_entry.pid<<" page_number:"<<lru_entry.page_number<<
            "dirty: " << lru_entry.dirty<<endl);
        return lru_entry;
    }
}global_lru_data;

void freeFrameManager();
int performPageIO(IOTableEntry& io_operation);
void pageIOManager();
void processT(int pid);
int mmu(int pid, int page_num);
int pageFaultHandler(int pid, int page_num,bool is_access);
string formatNumber(int num);

int main(){
    ifstream fin;
    string line;
    fin.open("init");
    vector<thread> pthreads;
    vector<int> list_of_pid;

    vector<thread> imp_threads;
    while(getline(fin,line)){
        vector<string> words;
        tokenize(line, words," ");
        string command = words[0];
        if(command == "end"){
            break;
        } else{
            vector <string> parameters;
            tokenize(words[1],parameters,",");
            if(command == "create"){
                int pid = stoi(parameters[0]);  //pid
                PInfo pi;
                pi.process_size = stoi(parameters[1]);  // size of process
                pdata[pid] = pi;
                pthreads.push_back(thread(processT,pid));
                list_of_pid.push_back(pid);
            } else if(command == "Memory_size"){
                RAM_size = stoi(parameters[0]);

                global_free_list.initialize();
                imp_threads.push_back(thread(pageIOManager));
                imp_threads.push_back(thread(freeFrameManager));
            } else if(command == "Lower_threshold"){
                l_thresh = stoi(parameters[0]);
            } else if(command == "Upper_threshold"){
                h_thresh = stoi(parameters[0]);
            } else if(command == "Page_table"){
                this_thread::sleep_for(chrono::seconds(1));
                for(int i = 0; i < list_of_pid.size(); i++){
                    PageTable* pt = global_page_table_list[list_of_pid[i]];
                    cout<<"\n--------page table of process:"<<list_of_pid[i]<<"------------\n";
                    cout<<"page no | frame no | valid | dirty\n";
                    pt->print_pt_table();
                }

            } else {
                cout << "invalid command";
            }
        }

    }

    for(auto it = pthreads.begin(); it!=pthreads.end();it++){
        it->join();
    }
    for(auto it = imp_threads.begin(); it!=imp_threads.end();it++){
        it->detach();
    }


}


int performPageIO(IOTableEntry& io_operation){
/*    auto lru_iterator = global_lru_data.backptr_pa_2_lru_iterator[io_operation.frame_no];
    global_lru_data.LRU_table.erase(lru_iterator);*/
    if(io_operation.page_in){
        debug("page in operation started: frame:"<<io_operation.frame_no<<" page no:"<<io_operation.page_no<<" pid"<<io_operation.pid<<endl);

        LRU_entry temp(io_operation.frame_no,
                        timer++,
                        io_operation.page_no,
                        io_operation.pid,
                        !io_operation.is_access);
        this_thread::sleep_for(chrono::milliseconds(100));
        // unique_lock<mutex> pt_locker(mutex_pt),lru_locker(mutex_lru_table),freelist_locker(mutex_free_list);
        PageTable* pt = global_page_table_list[io_operation.pid];
        PageEntry temp_page_entry(true, !io_operation.is_access, io_operation.frame_no);
        pt->addPageEntry(temp_page_entry,io_operation.page_no);
        global_lru_data.insertLRUEntry(io_operation.frame_no, temp);
        debug("page in for frame:"<<io_operation.frame_no<<" page:"<<io_operation.page_no<<" completed for pid:"<<io_operation.pid<<"\n");
        cond_page_i_completed.notify_all();
        // pt_locker.unlock();
        // lru_locker.unlock();
        // freelist_locker.unlock();
   }
    else if(io_operation.page_out){
        //clear the entry from processs pt
        PageTable* pt = global_page_table_list[io_operation.pid];
        pt->clear_entry(io_operation.page_no);
        //clear entry from lru table
        global_lru_data.eraseLRUEntry(io_operation.frame_no);

        debug("page out for frame:"<<io_operation.frame_no<<" page:"<<io_operation.page_no<<" completed for pid:"<<io_operation.pid<<"\n");
        global_free_list.addFreeFrame(io_operation.frame_no);
        cond_page_o_completed.notify_all();
    }
}

void pageIOManager(){
    while(true){
        unique_lock<mutex> io_locker(mutex_io_table);
        cond_io_task_arrived.wait(io_locker);
        debug("io manager got signal\n");
        while(!global_io_table.io_table.empty()){
            IOTableEntry io_operation = global_io_table.io_table.front();
            int result = performPageIO(io_operation);
            global_io_table.io_table.pop_front();
        }
        io_locker.unlock(); // to complete

    }
}

void freeFrameManager(){
    while(true){
        unique_lock<mutex> locker(mutex_free_list,defer_lock);
        locker.lock();
        cond_less_than_thres.wait(locker,[](){return global_free_list.free_list.size() < l_thresh;} );
        debug("signal for less then thresh recieved\n");
        locker.unlock();
        while(global_free_list.free_list.size() < h_thresh){
            LRU_entry to_remove = global_lru_data.getLRUEntryTORemove();

            if(to_remove.dirty){
                debug("frame choosen by free frame manager is dirty. frame:"<<to_remove.frame_no<<endl);

                debug("free frame manager initialising io\n");
                global_io_table.addIOTask(to_remove.page_number,
                                            to_remove.frame_no,
                                            to_remove.pid,
                                            false,
                                            true,false);

                locker.lock();
                cond_page_o_completed.wait(locker,
                                        [&to_remove](){return (global_lru_data.backptr_pa_2_lru_iterator.find(to_remove.frame_no) == global_lru_data.backptr_pa_2_lru_iterator.end());});

                locker.unlock();
            }
            else{
                debug("frame choosen by free frame manager is not dirty. frame:"<<to_remove.frame_no<<endl);
                PageTable* pt = global_page_table_list[to_remove.pid];
                pt->clear_entry(to_remove.page_number);
                global_lru_data.eraseLRUEntry(to_remove.frame_no);
                locker.lock();
                global_free_list.addFreeFrame(to_remove.frame_no);
                locker.unlock();
            }
        }

    }
}

int pageFaultHandler(int pid, int page_num, bool is_access){
	PageTable* pt = global_page_table_list[pid];
	LRU_entry lru_entry;

    int free_frame = global_free_list.getFreePage(pid);
    global_io_table.addIOTask(page_num,free_frame,pid,true,false,is_access);

    unique_lock<mutex> pt_locker(mutex_lru_table);
    cond_page_i_completed.wait(pt_locker,
                                [free_frame](){return global_lru_data.backptr_pa_2_lru_iterator.find(free_frame)!=global_lru_data.backptr_pa_2_lru_iterator.end();});

    debug("page fault handler imported page for frame:"<< free_frame<< " page:"<<page_num<<" pid:"<<pid<<endl);
    //LRU_entry lru_entry_for_i_page = *(global_lru_data.backptr_pa_2_lru_iterator[free_frame]);
    pt_locker.unlock();
    return 1;


}

int mmu(int pid, int page_num,bool is_access_instruction){
    if(page_num >= pdata[pid].process_size){
        debug("mem protection exception\n");
        return -1; //mem protection exception
    }

    PageTable* pagetable = global_page_table_list[pid];
    unique_lock<mutex> pt_locker(mutex_pt);
    PageEntry this_page = pagetable->page_entries[page_num];
    if(this_page.valid){
        debug("######" << page_num<< " " << this_page.frame_no);
        int update_result = global_lru_data.update(pid, this_page.frame_no, is_access_instruction);
        (pagetable->page_entries[page_num]).dirty = true;
        pt_locker.unlock();
        return this_page.frame_no;
    }
    pt_locker.unlock();
    debug("page entry not present in page table. page:"<<page_num<<" pid:"<<pid<<endl);
    int pagefault_handler_result = pageFaultHandler(pid,page_num,is_access_instruction);
    return -2;
}


void processT(int pid){
    ifstream infile("s"+to_string(pid));
    string command, page_no, word_no;
    int accessed = 0, modified = 0, pagefaults = 0;
    PageTable* pagetable = new PageTable(pid);
    global_page_table_list[pid] = pagetable;
    while (infile >> command){
        if(command == "end") {
            this_thread::sleep_for(chrono::seconds(1));
            cout<<"--------------------table for "<<pid<<"--------------------------\n";
        	cout<<"Number of access operations : " << formatNumber(accessed) << endl;
        	cout<<"Number of modify operations : " << formatNumber(modified) << endl;
        	cout<< "Number of page faults : " << formatNumber(pagefaults) << endl;
            //cout<<"---------------------------------------------------------\n";
            return ;
        }
        infile >> page_no >> word_no;
        bool is_access_instruction = false;
        cout<<pid << ": attempted to access/modify " << page_no << " " << word_no << endl;
        if(command == "access"){
            is_access_instruction = true;
            accessed++;
        }
        else if(command == "modify"){
            modified++;
        }
        int response = mmu(pid, stoi(page_no), is_access_instruction);

        if(response == -1){
            cout<<"Reported a memory protection violation "<< endl;
        }else if(response == -2){
            cout<<"Reported a page fault" << endl;
            pagefaults++;
        }else{
            cout<<"Accessed page frame number "<< response << endl;
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

void tokenize(const string& str,
            vector<string>& tokens,
            const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


