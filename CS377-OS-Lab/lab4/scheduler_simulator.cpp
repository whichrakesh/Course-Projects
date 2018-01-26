#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm> 
#include <unordered_map>
#include "event_manager.h"

#define debug(str) //{cout <<"DEBUG:"<< str << '\n';} 
using namespace std;

event_mgnr em; 
int current_time = 0;
bool DEBUG = false;
bool multilevel = false;
unordered_map<int, int> map_pid_level;
struct process;

struct scheduling_level {
	int level_no;
	int priority;
	int time_slice;	//either some integer or N
	bool is_done;
	queue<process*> process_list;
};

struct scheduler {
	int no_of_levels;
	vector<scheduling_level> levels_arr;
	int current_level;
	scheduler(){
        current_level = 0;
	}
	void addProcess(process* p);
};

scheduler Scheduler;

class comparator{
public:
 	int operator() ( const scheduling_level& p1, const scheduling_level &p2)
 	{
 		return p1.priority > p2.priority;
 	}
} compare;

struct process_phase {
	int itrs;	//number of iterations
	int cpu_b;	//cpu burst time
	int io_b;	//IO burst time
};

enum State {
	BLOCKED, RUNNING, READY, NOT_STARTED, TERMINATED
};



struct process {
	int pid;
	int start_priority;
	int admission;
	State state;
	int current_phase; //current phase
	int current_iteration; //current iteration in a phase
	int cpu_time_completed; // cpu time completed for the current iteration
	int current_start_time;
	vector<process_phase> phases;
	int checkTerminated(){
		return current_phase == phases.size();
	}
	void incrementPhaseCount(){
        this->current_iteration++;
        if(this->current_iteration == this->phases[current_phase].itrs){
            this->current_iteration = 0;
            this->current_phase++;
            if(this->current_phase == this->phases.size()){
                this->state = TERMINATED;
                if(multilevel)        	
	                if(Scheduler.levels_arr[map_pid_level[pid]].process_list.empty()){
	                	Scheduler.levels_arr[map_pid_level[pid]].is_done = true;
	                }
                cout << "PID :: " << pid << "  TIME :: " << current_time << "  EVENT :: Process terminated\n";
            }
        }
	}
	void start(){
        if(DEBUG)
            cout << current_time << ": process " << this->pid << " started\n";
        current_phase = 0;
        current_iteration = 0;
        cpu_time_completed = 0;
        state = READY;
	}
	int getNextIO(){
        return current_time + phases[current_phase].cpu_b - cpu_time_completed;
	}
    void admitProcess(){
        if(DEBUG)
            cout << current_time << ": process " << this->pid << " admitted\n";
        cout << "PID :: " << pid << "  TIME :: " << current_time << "  EVENT :: Process Admitted\n";
        this->start();
        if(multilevel){
            Scheduler.addProcess(this);
        }
    }
    void startIO(){
        if(DEBUG)
            cout << current_time << ": process " << this->pid << " IO started\n";
        cout << "PID :: " << pid << "  TIME :: " << current_time << "  EVENT :: IO started\n";
        this->state = BLOCKED;
        em.add_event(current_time + phases[current_phase].io_b, IO_END, this->pid);     
    }
    void endIO(){
        if(DEBUG)
            cout << current_time << ": process " << this->pid << " IO ended\n";
        cout << "PID :: " << pid << "  TIME :: " << current_time << "  EVENT :: IO burst completed\n";
        this->state = READY;
        cpu_time_completed = 0;
        this->incrementPhaseCount();
        if(multilevel)
        	Scheduler.addProcess(this);
    }
    void preempt(){
        if(DEBUG)
            cout << current_time << ": process " << this->pid << " preempted\n";
        cout << "PID :: " << pid << "  TIME :: " << current_time << "  EVENT :: Process pre-empted\n";
        this->state = READY;
        cpu_time_completed += current_time - current_start_time;
        if(multilevel){
            Scheduler.levels_arr[map_pid_level[pid]].process_list.pop();
            Scheduler.levels_arr[map_pid_level[pid]].process_list.push(this);
        }
    }
    void dispatch(int &next_io_t, int &next_timer_t){
        if(DEBUG)
            cout << current_time << ": process " << this->pid << " dispatched\n";
        cout << "PID :: " << pid << "  TIME :: " << current_time << "  EVENT :: CPU started\n";
        this->state = RUNNING;
        this->current_start_time = current_time;
        Scheduler.current_level = map_pid_level[this->pid];
        next_io_t = getNextIO();
        if(multilevel)
        	next_timer_t = current_time + Scheduler.levels_arr[Scheduler.current_level].time_slice;
    }
    void demote(){
        int level = map_pid_level[pid];        
        this->state = READY;
        cpu_time_completed += current_time - current_start_time;
        if(level < Scheduler.no_of_levels - 1){        	
        	Scheduler.levels_arr[map_pid_level[pid]].process_list.pop();
        	if(Scheduler.levels_arr[map_pid_level[pid]].process_list.empty()){
	        	Scheduler.levels_arr[map_pid_level[pid]].is_done = true;      	
	        }
            level++;
            this->start_priority = Scheduler.levels_arr[level].priority;
            Scheduler.levels_arr[level].process_list.push(this); 
            map_pid_level[this->pid]= level; 
            cout << "PID :: " << pid << "  TIME :: " << current_time << "  EVENT :: Priority demoted to level "<< Scheduler.levels_arr[level].level_no << "\n";
        } else {
        	Scheduler.levels_arr[map_pid_level[pid]].process_list.pop();
            Scheduler.levels_arr[map_pid_level[pid]].process_list.push(this);
        }                        
    }
    void promote(){
        int level = map_pid_level[pid];
        Scheduler.levels_arr[map_pid_level[pid]].process_list.pop();
        if(Scheduler.levels_arr[map_pid_level[pid]].process_list.empty()){
        	Scheduler.levels_arr[map_pid_level[pid]].is_done = true;      	
        }
        if(level > 0 && current_time - current_start_time < Scheduler.levels_arr[map_pid_level[pid]].time_slice){        	
            level--;
            cout << "PID :: " << pid << "  TIME :: " << current_time << "  EVENT :: Priority promoted to level "<< Scheduler.levels_arr[level].level_no << "\n";
            this->start_priority = Scheduler.levels_arr[level].priority;            
        }
        //Scheduler.addProcess(this);        
    }
};

vector<process> p_list;

process* getProcessByPID(int Pid){
    int i = 0;
    for(; i < p_list.size(); i++){
        if(p_list[i].pid == Pid)
            return &p_list[i];
    }
}

void handling_PROCESS_SPEC_file(){
	string line, line2;
	int pid, prior;
	int adm;
	int iter;
	int cpu_t, io_t;
	ifstream infile("PROCESS_SPEC");
	while (std::getline(infile, line))
	{
		if(line=="PROCESS"){
			process proc;
			getline(infile, line2);
			std::istringstream iss(line2);
		        if (!(iss >> pid >> prior >> adm)) { break; } // error

			proc.pid = pid;
			proc.start_priority = prior;
			proc.admission = adm;
            proc.state = NOT_STARTED;
			getline(infile, line2);
			while(line2 != "END"){
				std::istringstream iss(line2);
				process_phase pp;
			        if (!(iss >> iter >> cpu_t >> io_t)) { break; } // error

				pp.itrs = iter;
			    	pp.cpu_b = cpu_t;
			    	pp.io_b = io_t;
			    	(proc.phases).push_back(pp);
			    	getline(infile, line2);
			}
			p_list.push_back(proc);
			em.add_event(proc.admission,PROCESS_ADMISSION,proc.pid);	//event type "1" represents "process admission event"

		}
	}
}


void scheduler::addProcess(process* p){
	debug("in process")	;
	if(p->state == TERMINATED){
		return;
	}
    for(int i = 0; i < levels_arr.size(); i++){
        if(levels_arr[i].priority == p->start_priority){        	
            levels_arr[i].process_list.push(p);
            levels_arr[i].is_done = false;
            map_pid_level[p->pid]= i;            
            break;
        }
    }
}


int string_to_integer(string str){
	int r=1,s=0,l=str.length(),i;
	for(i=l-1;i>=0;i--)
	{
		s = s + ((str[i] - '0')*r);
		r *= 10;
	}
	return s;
}

void handling_SCHEDULER_SPEC_file(){
	string line, line2;
	int level_count;
	int prior;
	int s_lvl;
	int t_slice;
	string t_slice1;
	ifstream infile("SCHEDULER_SPEC");
	while (std::getline(infile, line))
	{
		if(line=="SCHEDULER"){
			getline(infile, line2);
			std::istringstream iss(line2);
		    if (!(iss >> level_count)) { break; } // error

			Scheduler.no_of_levels = level_count;
			for(int i=0; i<level_count; i++){
				getline(infile, line2);
				std::istringstream iss(line2);
				if (!(iss >> s_lvl >> prior >> t_slice1)) { break; } // error
				scheduling_level scl;
				if(t_slice1 == "N")
					t_slice = 9999;
				else
					t_slice = string_to_integer(t_slice1);
				scl.level_no = s_lvl;
				scl.priority = prior;
				scl.time_slice = t_slice;
				scl.is_done = true;
				Scheduler.levels_arr.push_back(scl);
			}
		}
	}
}
