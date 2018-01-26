#include "scheduler_simulator.cpp"

int scheduler();
int main(){
	handling_PROCESS_SPEC_file();
	handling_SCHEDULER_SPEC_file();
	//processing events
	//cout<<Scheduler.no_of_levels;
	multilevel = Scheduler.no_of_levels != 0;
	if(!multilevel){
		event next,top;
		int running_process_id = -1, next_process_id = -1;
		int next_io_t,next_timer_t;
		process* this_process, *running_process;
		while(!em.is_empty()){
			next = em.next_event();
			switch(next.type)
			{
            //routine for handling process admission event
				case PROCESS_ADMISSION:
				current_time = next.end_t;
				this_process = getProcessByPID(next.pid);
				this_process->admitProcess();
				if(next.pid == scheduler()){
					if(running_process_id != -1){
						running_process = getProcessByPID(running_process_id);
						running_process->preempt();
					}
					running_process_id = scheduler();
					if(running_process_id != -1){
						running_process = getProcessByPID(running_process_id);
						running_process->dispatch(next_io_t,next_timer_t);
					}
				}
				break;
            // case END_OF_ TIMESLICE:
            // 	cout << "PID :: " << next.pid << "  TIME :: " << next.end_t << "  EVENT ::  End of timeslice\n";
            // 	this_process = getProcessByPID(next.pid);
            // 	if(next.pid == running_process_id){
            // 		preempt(running_process_id);
            // 	}
            // 	running_process_id = scheduler();
            // 	dispatch(running_process_id);
            // 	break;
				case IO_START:
				cout << "PID :: " << next.pid << "  TIME :: " << next.end_t << "  EVENT :: CPU burst completed\n";
				current_time = next.end_t;
				this_process = getProcessByPID(next.pid);
				if(next.pid == running_process_id){
					this_process->startIO();
				}
				running_process_id = scheduler();
				if(running_process_id != -1){
					running_process = getProcessByPID(running_process_id);
					running_process->dispatch(next_io_t,next_timer_t);
				}
				break;
				case IO_END:
                //cout << "PID :: " << next.pid << "  TIME :: " << next.end_t << "  EVENT ::  IO Ended\n";
				current_time = next.end_t;
				this_process = getProcessByPID(next.pid);
				this_process->endIO();
				if(next.pid == scheduler()){
					if(running_process_id != -1){
						running_process = getProcessByPID(running_process_id);
						running_process->preempt();
					}
					running_process_id = scheduler();
					if(running_process_id != -1){
						running_process = getProcessByPID(running_process_id);
						running_process->dispatch(next_io_t,next_timer_t);
					}
				}
				break;
            //Define routines for other required events here.

			}
			top = em.top();
			if(running_process_id != -1 && (em.is_empty() || top.end_t > next_io_t) ) {
				em.add_event(next_io_t, IO_START, running_process_id);
			}
        //em.print();
		}
	}
	else{
		sort(Scheduler.levels_arr.begin(),Scheduler.levels_arr.end(),compare);
		event next,top;
		int running_process_id = -1, next_process_id = -1;
		int next_io_t, next_timer_t ;
		process* this_process, *running_process;
		while(!em.is_empty()){
			next = em.next_event();
			switch(next.type)
			{
            //routine for handling process admission event
				case PROCESS_ADMISSION:
					current_time = next.end_t;
					this_process = getProcessByPID(next.pid);
					this_process->admitProcess();

					if(next.pid == scheduler()){
						if(running_process_id != -1){
							running_process = getProcessByPID(running_process_id);
							running_process->preempt();
						}                        
						running_process_id = scheduler();
						if(running_process_id != -1){
							running_process = getProcessByPID(running_process_id);
							running_process->dispatch(next_io_t,next_timer_t);
						}
					}
					break;
				case END_OF_TIMESLICE:
					cout << "PID :: " << next.pid << "  TIME :: " << next.end_t << "  EVENT :: Time slice ended\n";
					current_time = next.end_t;
					this_process = getProcessByPID(next.pid);
					if(next.pid == running_process_id){
						this_process->demote();
					}
					running_process_id = scheduler();
					if(running_process_id != -1){
						running_process = getProcessByPID(running_process_id);
						running_process->dispatch(next_io_t,next_timer_t);
						next_timer_t = current_time + Scheduler.levels_arr[Scheduler.current_level].time_slice;
					}
					break;
				case IO_START:
					cout << "PID :: " << next.pid << "  TIME :: " << next.end_t << "  EVENT :: CPU burst completed\n";
					current_time = next.end_t;
					this_process = getProcessByPID(next.pid);
					if(next.pid == running_process_id){
						this_process->startIO();
						this_process->promote();
					}
					running_process_id = scheduler();
					if(running_process_id != -1){
						running_process = getProcessByPID(running_process_id);
						running_process->dispatch(next_io_t,next_timer_t);                        
					}
					break;
				case IO_END:
	                //cout << "PID :: " << next.pid << "  TIME :: " << next.end_t << "  EVENT ::  IO Ended\n";
					current_time = next.end_t;
					this_process = getProcessByPID(next.pid);
					this_process->endIO();
					if(next.pid == scheduler()){
						if(running_process_id != -1){
							running_process = getProcessByPID(running_process_id);
							running_process->preempt();
						}
						running_process_id = scheduler();
						if(running_process_id != -1){
							running_process = getProcessByPID(running_process_id);
							running_process->dispatch(next_io_t,next_timer_t);
						}
					}
					break;
            //Define routines for other required events here.
			}
			top = em.top();     
			if(next_io_t <= next_timer_t){
				if(running_process_id != -1 && (em.is_empty() || top.end_t > next_io_t) ) {
					em.add_event(next_io_t, IO_START, running_process_id);
				}				
			} else {
				if(running_process_id != -1 && (em.is_empty() || top.end_t > next_timer_t) ) {            	
					em.add_event(next_timer_t, END_OF_TIMESLICE, running_process_id);
				}
			}			
			//em.print();
			//debug("hello");
		}
	}
	return 0;

}


int scheduler(){
	int next_pid = -1;
	int max_priority = -1;
	if(!multilevel){
		for(vector<process>::iterator it = p_list.begin(); it != p_list.end(); it++ ){
			if(it->start_priority > max_priority && (it->state == READY || it->state == RUNNING)){
				max_priority = it->start_priority;
				next_pid = it->pid;
			}
		}
	} else {
		for(vector<scheduling_level>::iterator it = Scheduler.levels_arr.begin(); it != Scheduler.levels_arr.end(); it++ ){
			//cout << it->level_no << "\n"; 
			if(!it->is_done){            	
				next_pid = it->process_list.front()->pid;
				break; 				              			
			}
		}
	}
	return next_pid;
}
