#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <set>
#define SHMSZ 27
#define DEBUG false
key_t key = 5678;

using namespace std;
struct Child{
	int pid;
	string domain;
	Child(int pid,string domain){
		this->pid = pid;
		this->domain = domain;
	}
};
int findChildProcess(string domain,vector<Child> children);
void my_handler(int signum);
void my_parent_handler(int signum);
void childProcess();
void printChildren(vector<Child> children);
static vector <Child> children;



//----------------------------------------------------------------------------------------------------------------------
int main(){
	if(DEBUG)
		cout<< "parent process =" <<getpid()<<endl;	
	string command;
	char c;
    int shmid;    
    char *shm, *s;
	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((shm = (char*) shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    s = shm;
    struct sigaction sig;
	sig.sa_flags = 0;
	sig.sa_handler = &my_parent_handler;
	sigaction(SIGUSR1,&sig,NULL);

	while(getline(cin,command)){
		if(DEBUG){
			cout << "command entered = " << command << endl;
		}
		size_t spacepos = command.find(" ");
		string command_code =  command.substr(0,spacepos) ;
		if(command_code == "add_email"){
			string email = command.substr(spacepos+1);
			//cout<< "email:" << email<<endl;
			string domain = email.substr(email.find("@")+1);
			//cout<< "domain:" << domain<<endl;
			if(DEBUG){
				printChildren(children);
			}			
			int pid = findChildProcess(domain,children);
			const char* text = command.c_str();
			if(pid == -1){
				int temp_id = fork();
				if(temp_id == 0){	
					childProcess();
					_exit(0);
				} else {
					Child child(temp_id,domain);
					children.push_back(child);	
					sleep(1);				
					for (s = shm ; *text != '\0'; s++){					
				        *s = *text;
				        text++;
					}
				    *s = '\0';				    
				    kill(temp_id,SIGUSR1);	
				}
			} else {
				sleep(1);
				for (s = shm ; *text != '\0'; s++){					
				        *s = *text;
				        text++;
					}
				*s = '\0';	
				kill(pid,SIGUSR1);
			}
		}
		if(command_code == "search_email"){
			string email = command.substr(spacepos+1);
			string domain = email.substr(email.find("@")+1);
			int pid = findChildProcess(domain,children);
			int temp_id = 0;
			const char* text = command.c_str();
			if(pid == -1){
				cout<< "Parent process - Domain does not exist." << endl;
			} else {
				sleep(1);				
				for (s = shm ; *text != '\0'; s++){					
				        *s = *text;
				        text++;
					}
				*s = '\0';					
				kill(pid,SIGUSR1);
			}
		}
		if(command_code == "delete_email"){
			string email = command.substr(spacepos+1);
			string domain = email.substr(email.find("@")+1);
			int pid = findChildProcess(domain,children);
			int temp_id = 0;
			const char* text = command.c_str();
			if(pid == -1){
				cout<< "Parent process - Domain does not exist." << endl;
			} else {
				sleep(1);				
				for (s = shm ; *text != '\0'; s++){					
				        *s = *text;
				        text++;
					}
				*s = '\0';					
				kill(pid,SIGUSR1);
			}
		}
		if(command_code == "delete_domain"){
			string domain = command.substr(spacepos+1);
			int pid = findChildProcess(domain,children);
			const char* text = command.c_str();
			if(pid == -1){
				cout<< "Parent process - Domain does not exist." << endl;
			} else {
				sleep(1);				
				for (s = shm ; *text != '\0'; s++){					
				        *s = *text;
				        text++;
					}
				*s = '\0';					
				kill(pid,SIGUSR1);
				sleep(1);
				cout << "Parent process - Domain " + domain + " with PID - " << pid << " deleted." << endl;
			}
		}
		if(command_code == "send_email"){			
			string emails_body = command.substr(spacepos+1);
			string first_email = emails_body.substr(0,emails_body.find(" "));
			string second_email_body = emails_body.substr(emails_body.find(" ")+1);
			string second_email = second_email_body.substr(0,second_email_body.find(" "));
			string body = second_email_body.substr(second_email_body.find(" ")+1);
			string first_domain = first_email.substr(first_email.find("@")+1);
			string second_domain = second_email.substr(second_email.find("@")+1);

			int first_pid = findChildProcess(first_domain,children);
			int second_pid = findChildProcess(second_domain,children);
			string message = command_code + " " + to_string(second_pid) + " "+ emails_body;
			const char* text = message.c_str();
			if(first_pid == -1 || second_pid == -1){
				cout<< "Parent process - The sender/receiver does not exist" << endl;
			} else {				
				sleep(1);				
				for (s = shm ; *text != '\0'; s++){					
				        *s = *text;
				        text++;
					}
				*s = '\0';					
				kill(first_pid,SIGUSR1);
			}
		}	
		sleep(0.5);	

	}	
}
void printVector(vector<string> data){
	for(int i=0;  i < data.size(); i++){
		cout<<data[i]<<" ";
	} cout << endl;
}
void my_parent_handler(int signum){
	if(DEBUG)
		cout << "parent handler called" << endl;
	int shmid;	
    char *shm, *s;
	if ((shmid = shmget(key, SHMSZ,  0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((shm = (char*) shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    string message = "";
    for (s = shm; *s != '\0'; s++)
        message += *s;    
    cout<< "[parent]" << message <<endl;
}
void my_handler(int signum){
	int shmid;
    char *shm, *s;
    static set<string> data;
	if ((shmid = shmget(key, SHMSZ,  0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((shm = (char*) shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    //cout << "lol" << endl;
    string command = "";
    for (s = shm; *s != '\0'; s++)
        command += *s;    
    //cout<< "["<< getpid() << "]command recevied at child: " << command <<endl;
    string command_code =  command.substr(0,command.find(" ")) ;    
    string message;
    if(command_code == "add_email"){
    	string email = command.substr(command.find(" ")+1);
	    string domain = email.substr(email.find("@")+1);
	    string username = email.substr(0,email.find("@"));
    	if(data.find(username) != data.end()){
    		message = "Child process " + domain + " - Email address already exists.";
    	} else {
    		data.insert(username);
    		message = "Child process " + domain + " - Email address " + email + " added successfully.";    		
    	}    	
    }
    if(command_code == "search_email"){
    	string email = command.substr(command.find(" ")+1);
	    string domain = email.substr(email.find("@")+1);
	    string username = email.substr(0,email.find("@"));
    	set<string>::iterator it = data.find(username);
    	if(it != data.end()){
    		message = "Parent process - found the email address " + email + " at " + to_string(distance(data.begin(), it));
    	} else {
    		message = "Parent process - could not find the email address " + email;    		
    	} 
	}
	if(command_code == "delete_email"){
		string email = command.substr(command.find(" ")+1);
	    string domain = email.substr(email.find("@")+1);
	    string username = email.substr(0,email.find("@"));
		set<string>::iterator it = data.find(username);
    	if(it != data.end()){
    		message = "Child process - child "+ domain +" deleted "+ email +" from position " + to_string(distance(data.begin(), it));
    	} else {
    		message = "Parent process - could not find the email address " + email;    		
    	} 
	}
	if(command_code == "delete_domain"){
		string domain = command.substr(command.find(" ")+1);
		cout<< "usernames for the domain " << domain <<":";
		for(set<string>::iterator it = data.begin(); it != data.end(); it++){
			cout << *it << " ";
		}
		cout << endl;
		kill(getpid(),SIGTERM);		
	}
	if(command_code == "send_email"){
		string pid_emails_body = command.substr(command.find(" ")+1);
		string pid_text = pid_emails_body.substr(0,pid_emails_body.find(" "));
		string emails_body = pid_emails_body.substr(pid_emails_body.find(" ")+1);
		string first_email = emails_body.substr(0,emails_body.find(" "));
		string second_email_body = emails_body.substr(emails_body.find(" ")+1);
		string second_email = second_email_body.substr(0,second_email_body.find(" "));
	    string username = first_email.substr(0,first_email.find("@"));		
		int pid = stoi(pid_text);
		set<string>::iterator it = data.find(username);
    	if(it != data.end()){
			string message = "sent_email "+  emails_body;
			const char* text = message.c_str();
			sleep(1);				
			for (s = shm ; *text != '\0'; s++){					
			        *s = *text;
			        text++;
				}
			*s = '\0';					
			kill(pid,SIGUSR1);
			cout << "["<< getpid() << "] Child Process - Email sent from " << first_email << " to "<< second_email << endl;
		} else {
			cout << "["<< getpid() << "] Could not send email (Username:" << username << " doesnot exist.)" << endl;
		}

	}
	if(command_code == "sent_email"){
		string emails_body = command.substr(command.find(" ")+1);
		string first_email = emails_body.substr(0,emails_body.find(" "));
		string second_email_body = emails_body.substr(emails_body.find(" ")+1);
		string second_email = second_email_body.substr(0,second_email_body.find(" "));
		string body = second_email_body.substr(second_email_body.find(" ")+1);
		string username = second_email.substr(0,second_email.find("@"));
		set<string>::iterator it = data.find(username);
    	if(it != data.end()){
			cout <<  "["<< getpid() << "] Child Process - Email received at process " << getpid() << " for user " << username << endl;
		} else {
			cout << "["<< getpid() << "] " << username << " doesnot exist." << endl;
		}
	}		    		    
	if(message != ""){
		if(DEBUG){
	    	cout<< "[" << getpid() << "] message sent from child:" << message << endl;
	    }
		const char* text = message.c_str();
	    for (s = shm ; *text != '\0'; s++){					
		        *s = *text;
		        text++;
			}
		*s = '\0';
		kill(getppid(),SIGUSR1);
	}							   
    //printVector(data);
}
void childProcess(){
	if(DEBUG)
		cout<<"child process created: pid =" << getpid() << endl;
	struct sigaction sig;
	sig.sa_flags = 0;
	sig.sa_handler = &my_handler;
	sigaction(SIGUSR1,&sig,NULL);
	while(true){		
		sleep(10);
	}
}

int findChildProcess(string domain,vector<Child> children){
	for(vector<Child>::iterator it = children.begin(); it != children.end(); it++){
		if(it->domain == domain){
			// cout<<"pid =" << it->pid<<endl;
			return it->pid;
		}
	}
	return -1;
}

void printChildren(vector<Child> children){
	for(vector<Child>::iterator it = children.begin(); it != children.end(); it++){
		cout<<it->domain << ":" << it->pid << endl;
	}	
}
