#include <iostream>
#include <vector>
#include <queue>

using namespace std;

enum EventType{
	PROCESS_ADMISSION = 1,
	END_OF_TIMESLICE,
	IO_START,
	IO_END
};

struct event{
	int end_t;	//event occurrence time
	int type;	//event type	
	int pid;	//process id
};

class comp{
public:
 	int operator() ( const event& p1, const event &p2)
 	{
 		return p1.end_t>p2.end_t;
 	}
};

class event_mgnr {
	public:
		priority_queue<event, vector<event>, comp> event_table;

	//function for adding an event in the event table
	void add_event(int end_t, int type, int pid)
	{
		event ev;
		ev.end_t = end_t;
		ev.type = type;
		ev.pid = pid;
		event_table.push(ev);
	}

	//Is event table empty..?
	bool is_empty()
	{
		return event_table.empty();
	}

	//function for returning the top_most entry of the event table
	event next_event()
	{
		event ev = event_table.top();
		event_table.pop();
		return ev;
	}

	event top(){
        event ev = event_table.top();
        return ev;
	}
	void print(){
        priority_queue<event, vector<event>, comp> event_copy = event_table;
        while(!event_copy.empty()){
            cout << "time:" << event_copy.top().end_t <<" PID: " << event_copy.top().pid << " type: " << event_copy.top().type << " ;";
            event_copy.pop();
        }
        cout <<endl;
	}

};
