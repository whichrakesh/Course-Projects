#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <map>
#include <mutex>
#include <list>
#include <vector>

#define BLOCK_SIZE 256
#define BLOCKS_COUNT 1000
#define DEBUG 1

#define println(X) { cout << X<<endl;}
#define debug(X) { if(DEBUG) cout << X<<endl;}
#define error(X) { cerr << X << endl;}
#define min(X,Y) (X<Y?X:Y)
#define max(X,Y) (X>Y?X:Y)

using namespace std;

mutex mutex_disk,mutex_superblock;

int toInt(string str);
string addressToString(int address);
void stringToCharArray(string str, char* ch, char fill_char);
void tokenize(const string& str,
            vector<string>& tokens,
            const string& delimiters);
struct SuperBlock{
	unsigned int blocks_count,root_dir_block_no,first_free_block;
	SuperBlock(int b, int r, int l){
		blocks_count = b;
		root_dir_block_no = r;
		first_free_block = l;
	}
	SuperBlock(){

	}

	SuperBlock(string block){
		vector<string> words;
		tokenize(block, words,",");
		blocks_count = stoi(words[0]);
		root_dir_block_no = stoi(words[1]);
		first_free_block = stoi(words[2]);
	}

	unsigned int getFirstFreeBlockNo(){
		return first_free_block;
	}

	void print(){
		debug("SuperBlock: " << blocks_count << " " << root_dir_block_no << " " << first_free_block);
	}

	string toString(){
		return to_string(blocks_count) + "," + to_string(root_dir_block_no) + "," + to_string(first_free_block);
	}
};

bool diskExists(){
	struct stat buffer;   
  	return (stat ("disk", &buffer) == 0); 
}

class Disk{
	FILE *fp;
public:
	~Disk(){
		fclose(fp);
	}
	void initialize(){
		if(diskExists())
			fp = fopen("disk","r+");
		else
			fp = fopen("disk","w+");
	}
	void read(int block_no, char *block ){
		//debug("read disk called");
		long int seek_addr = block_no * BLOCK_SIZE;
		fseek(fp,seek_addr,SEEK_SET);
		//fgets(block,BLOCK_SIZE,fp);
		for(int i=0; i< BLOCK_SIZE; i++){
			block[i] = (char) fgetc(fp);
		}
	}
	void write(int block_no, char* block){
		//debug("write disk called with " << block); 
		long int seek_addr = block_no * BLOCK_SIZE;
		fseek(fp,seek_addr,SEEK_SET);
		//fputs(block,fp);
		for(int i=0; i< BLOCK_SIZE; i++){
			fputc(block[i], fp);
		}
	}
};

struct Permission{
    bool write_p, read_p;
    Permission(bool w, bool r){
        write_p = w;
        read_p = r;
    }
    Permission(){}

    Permission(string str){
    	// debug("string got " << str);
		vector<string> words;
		tokenize(str, words,",");		
		write_p = stoi(words[0]);
		read_p = stoi(words[1]);
	}

    string toString(){
		return to_string(write_p) + "," + to_string(read_p);
	}
};

struct DirectoryEntry{
    string name ;
    char type;
    int location, size;
    bool locked;
    map<string, Permission> protection_info;
    DirectoryEntry(string n, char t, int l, int s, map<string, Permission> pi){
    	name = n;
    	type = t;
    	location = l;
    	locked = false;
    	size = s;
    	protection_info = pi;
    }
    DirectoryEntry(){}
    DirectoryEntry(string str){
    	// debug("Directory entry string:"<< str);
    	vector<string> words;
		tokenize(str, words,";");
		name = words[0];
		type = words[1][0];
		location = stoi(words[2]);
		size = stoi(words[3]);
		locked = stoi(words[4]);
		vector<string> p_info;
		tokenize(words[5], p_info, " ");
		// debug("pinfo string:"<< words[5]);
		for(int i = 0; i < p_info.size(); i++){	
			vector<string> rights;		
			tokenize(p_info[i],rights,":");
			// debug("pinfo string:"<< rights[1]);
			protection_info[rights[0]] = Permission(rights[1]);
		}
    }
    string toString(){
    	string str = name + ";" + type + ";" + to_string(location) + ";" + 
    				to_string(size) + ";" + to_string(locked) + ";";    			
		for(auto it = protection_info.begin(); it != protection_info.end(); it++){			
			str += it->first + ":" +  (it->second).toString() + " ";			
		}
		str[str.size()-1] = ';';
		return str;
    }
    bool isLocked(){
    	return locked;
    }
    void lock(){
    	locked = true;
    }
    void unlock(){
    	locked = false;
    }
};

struct Directory{
    list<DirectoryEntry> directory_entries;
    Directory(){}
    Directory(string str){
    	vector<string> words;
		tokenize(str, words,"\n");
		for(int i=0; i<words.size(); i++){
			DirectoryEntry de = DirectoryEntry(words[i]);
			directory_entries.push_back(de);
		}
    }    
    string toString(){
    	string str;
    	for(auto it = directory_entries.begin(); it != directory_entries.end(); it++){
    		str += it->toString() + "\n";
    	}
    	return str;
    }
    bool fileOrDirExists(string name){
    	for(auto it = directory_entries.begin(); it != directory_entries.end(); it++){
    		if(it->name == name){
    			return true;
    		}
    	}
    	return false;
    }
    int getDirLocation(string dir_name){
    	for(auto it = directory_entries.begin(); it != directory_entries.end(); it++){
    		if(it->name == dir_name && it->type == 'd'){
    			return it->location;
    		}
    	}
    	return -1;
    }
    int getFileLocation(string file_name){
    	for(auto it = directory_entries.begin(); it != directory_entries.end(); it++){
    		if(it->name == file_name && it->type == 'f'){
    			return it->location;
    		}
    	}
    	return -1;
    }

    int lockFile( string file_name){
    	for(auto it = directory_entries.begin(); it != directory_entries.end(); it++){
    		if(it->name == file_name && it->type == 'f'){
    			if(it->isLocked())
    				return -1;
    			else{
    				it->lock();
    				return 0;
    			} 
    		}
    	}
    	return -2;
    }
    int unlockFile(string file_name){
    	for(auto it = directory_entries.begin(); it != directory_entries.end(); it++){
    		if(it->name == file_name && it->type == 'f'){
    			if(it->isLocked()){
    				it->unlock();
    				return 0;
    			}
    			else{
    				return -1;
    			} 
    		}
    	}
    	return -2;
    }    
    int getFileSize(string file_name){
    	for(auto it = directory_entries.begin(); it != directory_entries.end(); it++){
    		if(it->name == file_name && it->type == 'f'){
    			return it->size;
    		}
    	}
    	return -1;
    }

    void setFileSize(string file_name, int size){
    	for(auto it = directory_entries.begin(); it != directory_entries.end(); it++){
    		if(it->name == file_name && it->type == 'f'){
    			it->size = size;
    		}
    	}
    }
};

struct FCB{
	string file_name;
	int file_addr,cur_position,parent_dir_addr;
	FCB(string name, int pos, int addr, int p_addr){
		file_name = name;
		cur_position = pos;
		file_addr = addr;
		parent_dir_addr = p_addr;
	}
	FCB(){}
};

struct User{
    string current_directory, user_name;
    int cur_dir_address;
    User(string uname,int dir_address){
    	debug("user:" << uname);
        user_name = uname;        
        current_directory = "/root/"+user_name+"/";
        cur_dir_address = dir_address;
    }
};

class FileSystem{
	Disk disk;
	SuperBlock superblock;
	Directory root_dir;
	map<string, User*> user_map;
public:
	void initialize(){
		char block[BLOCK_SIZE];	
		lock_guard<mutex> disk_locker(mutex_disk);	
		if(diskExists()){
			debug("disk exists");
			disk.initialize();
			disk.read(0,block);
			superblock = SuperBlock(string(block));
			superblock.print();
			disk.read(1,block);
			root_dir = Directory(string(block));
			for(list<DirectoryEntry>::iterator it = root_dir.directory_entries.begin(); it != root_dir.directory_entries.end(); it++){				
				user_map[it->name] = new User(it->name,it->location);
			}
		} else {
			debug("disk does not exist");
			disk.initialize();
			SuperBlock sb(BLOCKS_COUNT,1,1);
			stringToCharArray(sb.toString(),block,'\0');
			disk.write(0,block);
			superblock = sb;
			superblock.print();
			string s = root_dir.toString();	
			debug("root dir entries = " << s);
			stringToCharArray(s,block,'\0');
			disk.write(1,block);
			superblock.first_free_block++;	
			updateSuperBlock();
		}
	}
	void updateSuperBlock(){
		char block[BLOCK_SIZE];		
		stringToCharArray(superblock.toString(),block,'\0');
		disk.write(0,block);
		debug("superblock updated. first_free_block=" << superblock.first_free_block);
	}
	bool userExists(string uname){
	    if(user_map.count(uname) != 0)
	        return true;
	    return false;
	}
	void createUser(string uname){
		char block[BLOCK_SIZE];	
		Directory dir;	// creating empty directory
		stringToCharArray(dir.toString(),block,'\0');
		unique_lock<mutex> disk_locker(mutex_disk);
		int dir_loc = superblock.getFirstFreeBlockNo();
		disk.write(dir_loc,block);	// creating a directory in disk		
		superblock.first_free_block++;	
		updateSuperBlock();
		map<string, Permission> m;
		m[uname] = Permission(true,true);
		DirectoryEntry user_dir_entry(uname,'d', dir_loc, 0 , m);
		root_dir.directory_entries.push_back(user_dir_entry);
		// memcpy(block,&root_dir,sizeof(Directory));
		stringToCharArray(root_dir.toString(),block,'\0');
		debug("root dir content changed to " << block);
		disk.write(superblock.root_dir_block_no,block); // updating root directory entries
		disk_locker.unlock();
	    user_map[uname] = new User(uname,dir_loc);
	    debug("user " << uname << " created.");
	}
	void checkUser(string uname){
		if(!userExists(uname)){
			createUser(uname);
		}
	}
	void makeDirectory(string uname, string dir_name){
		if(userExists(uname)){
			User* user = user_map[uname]; 
			char block[BLOCK_SIZE];				
			disk.read(user->cur_dir_address,block);
			Directory cur_dir = Directory(string(block)); // retrieving current directory
			if(cur_dir.fileOrDirExists(dir_name)){
				error("Already a file/dir with name:" << dir_name << " exists");
				return;
			}
			Directory dir;	// creating empty dir
			stringToCharArray(dir.toString(),block,'\0');
			unique_lock<mutex> disk_locker(mutex_disk);
			int dir_loc = superblock.getFirstFreeBlockNo();
			disk.write(dir_loc,block);	// creating a directory in disk		
			superblock.first_free_block++;	
			updateSuperBlock();
			map<string, Permission> m;
			m[uname] = Permission(true,true);
			DirectoryEntry user_dir_entry(dir_name,'d', dir_loc, 0 ,  m);
			cur_dir.directory_entries.push_back(user_dir_entry);
			// memcpy(block,&root_dir,sizeof(Directory));
			stringToCharArray(cur_dir.toString(),block,'\0');
			debug("current dir content changed to " << block);
			disk.write(user->cur_dir_address,block); // updating current directory entries
			disk_locker.unlock();
		} else {
			error("No user with username: " << uname <<" exists");
			return;
		}
	}
	void changeCurrentDirectory(string uname, string dir_name){
		if(userExists(uname)){
			User* user = user_map[uname]; 
			char block[BLOCK_SIZE];				
			disk.read(user->cur_dir_address,block);
			Directory cur_dir = Directory(string(block)); // retrieving current directory	
			int addr = cur_dir.getDirLocation(dir_name);
			if(addr == -1){
				error("No dir with name:" << dir_name << " exists in the current directory.");
				return;
			}
			user->cur_dir_address = addr;
			user->current_directory += dir_name + "/";
			println("current directory of user:" << uname << " is " << user->current_directory);
		} else {
			error("No user with username: " << uname <<" exists");
			return;
		}
	}
	int createFile(string uname, int cur_dir_addr, Directory &cur_dir, string file_name){
		char block[BLOCK_SIZE];
		stringToCharArray("-01-01",block,'\0');
		int file_loc = superblock.getFirstFreeBlockNo();
		disk.write(file_loc,block);	// creating a file in disk		
		superblock.first_free_block++;	
		updateSuperBlock();
		map<string, Permission> m;
		m[uname] = Permission(true,true);
		DirectoryEntry file_entry(file_name,'f', file_loc, 0 , m);
		cur_dir.directory_entries.push_back(file_entry);
		stringToCharArray(cur_dir.toString(),block,'\0');
		debug("current dir content changed to " << block);
		disk.write(cur_dir_addr,block); // updating current directory entries		
		return file_loc;
	}
	int openFile(string uname, string file_path, FCB &fcb){
		vector<string> words;
		tokenize(file_path, words,"/");
		Directory cur_dir = root_dir;
		int cur_dir_addr;
		char block[BLOCK_SIZE];	
		if(words.size() < 2){
			error("insufficient file path length");
			return -1;
		}
		if(words[1] != uname){
			error(uname << " is not authorized to access file " << file_path);
			return -2;
		}
		lock_guard<mutex> disk_locker(mutex_disk);
		for(int i=1; i < words.size()-1; i++){
			string next_dir = words[i];
			cur_dir_addr = cur_dir.getDirLocation(next_dir);			
			if(cur_dir_addr == -1){
				error("Invalid file path name");
				return -3;
			}
			disk.read(cur_dir_addr,block);
			cur_dir = Directory(string(block));			
		}
		string file_name = words[words.size()-1];
		int addr = cur_dir.getFileLocation(file_name); // getting file address from current directory
		if(addr == -1){
			println("file not present. creating file:" << file_name);
			addr = createFile(uname,cur_dir_addr,cur_dir,file_name);
			cur_dir.lockFile(file_name);
			stringToCharArray(cur_dir.toString(),block,'\0');
			disk.write(cur_dir_addr,block);
			fcb = FCB(file_name,0,addr,cur_dir_addr);
			return 0;
		} else {
			int res = cur_dir.lockFile(file_name);
			if(res != 0) {
				println("file " << file_name << " is currently opened by another process.");				
				return -1;
			}
			stringToCharArray(cur_dir.toString(),block,'\0');
			disk.write(cur_dir_addr,block);			
			fcb = FCB(file_name,0,addr,cur_dir_addr);
			return 0;
		}
	}
	int writeFile(FCB &fcb, int bytes_count, string value){
		//file_size;	
		int seek = fcb.cur_position;	
		int block_no = 	seek/(BLOCK_SIZE-6); // 6 for metadata in file blocks
		int byte_no = seek%(BLOCK_SIZE-6);
		debug("seek block and byte: " << block_no << " " << byte_no);
		char block[BLOCK_SIZE];
		lock_guard<mutex> disk_locker(mutex_disk);
		disk.read(fcb.parent_dir_addr,block);
		Directory parent_dir = Directory(string(block));
		int file_size = parent_dir.getFileSize(fcb.file_name);
		int next_block_no = fcb.file_addr;//dummy
		int prev_block_no = 0;
		for(int i=0; i<block_no ; i++){
			disk.read(next_block_no,block);
			prev_block_no = next_block_no;
			next_block_no = toInt(string(block).substr(3,3));
			if(next_block_no == -1){
				next_block_no = newFileBlock(prev_block_no);					
			}
		}
		disk.read(next_block_no,block);
		int j=byte_no+6;
		int min= min(bytes_count,value.length());
		// debug("min=" << min);
		for(int i=0; i<min; i++){
			block[j] = value[i];
			j++;
			if(j >= BLOCK_SIZE){// block got full
				disk.write(next_block_no,block);
				next_block_no = newFileBlock(next_block_no);
				disk.read(next_block_no,block);
				j = 6;
			}
		}
		while(j < BLOCK_SIZE){ //appending block with null character
			block[j]='\0';
			j++;
		}
		disk.write(next_block_no,block); // updating last block
		fcb.cur_position = seek+min(bytes_count,value.length());
		file_size = max(seek+min(bytes_count,value.length()),file_size); // updating file size 		
		parent_dir.setFileSize(fcb.file_name,file_size);
		stringToCharArray(parent_dir.toString(),block,'\0');
		disk.write(fcb.parent_dir_addr,block);
	}
	int fileSeek(FCB &fcb, int seek){		
		char block[BLOCK_SIZE];
		lock_guard<mutex> disk_locker(mutex_disk);
		disk.read(fcb.parent_dir_addr,block);
		Directory parent_dir = Directory(string(block));
		int file_size = parent_dir.getFileSize(fcb.file_name);
		int block_no = 	file_size/(BLOCK_SIZE-6); // 6 for metadata in file blocks
		int byte_no = file_size%(BLOCK_SIZE-6);
		debug("file last block and byte: " << block_no << " " << byte_no);
		fcb.cur_position = seek;
		if(seek < file_size) // no need to append space
			return 0;
		int next_block_no = fcb.file_addr;//dummy
		int prev_block_no = 0;
		for(int i=0; i<block_no ; i++){
			disk.read(next_block_no,block);
			prev_block_no = next_block_no;
			next_block_no = toInt(string(block).substr(3,3));
			if(next_block_no == -1){
				next_block_no = newFileBlock(prev_block_no);					
			}
		}
		disk.read(next_block_no,block);
		int j=byte_no+6;
		for(int i=0; i<(seek-file_size); i++){
			block[j] = ' ';
			j++;
			if(j >= BLOCK_SIZE){// block got full
				disk.write(next_block_no,block);
				next_block_no = newFileBlock(next_block_no);
				disk.read(next_block_no,block);
				j = 6;
			}
		}
		while(j < BLOCK_SIZE){ //appending block with null character
			block[j]='\0';
			j++;
		}
		disk.write(next_block_no,block); // updating last block
		file_size = seek;
		parent_dir.setFileSize(fcb.file_name,file_size);
		stringToCharArray(parent_dir.toString(),block,'\0');
		disk.write(fcb.parent_dir_addr,block);
	}
	string readFile(FCB &fcb, int bytes_count){
		int seek = fcb.cur_position;	
		int block_no = 	seek/(BLOCK_SIZE-6); // 6 for metadata in file blocks
		int byte_no = seek%(BLOCK_SIZE-6);
		debug("read seek block and byte: " << block_no << " " << byte_no);
		char block[BLOCK_SIZE];
		lock_guard<mutex> disk_locker(mutex_disk);
		disk.read(fcb.parent_dir_addr,block);
		Directory parent_dir = Directory(string(block));
		int file_size = parent_dir.getFileSize(fcb.file_name);
		int next_block_no = fcb.file_addr;//dummy
		int prev_block_no = 0;
		for(int i=0; i<block_no ; i++){
			disk.read(next_block_no,block);
			prev_block_no = next_block_no;
			next_block_no = toInt(string(block).substr(3,3));
			if(next_block_no == -1){
				return "";					
			}
		}
		disk.read(next_block_no,block);
		int j=byte_no+6;
		string output;
		int min = min(bytes_count,file_size-seek);		
		for(int i=0; i<min; i++){
			output += block[j];
			j++;
			if(j >= BLOCK_SIZE){// block got passed
				next_block_no = toInt(string(block).substr(3,3));
				disk.read(next_block_no,block);
				j = 6;
			}
		}
		fcb.cur_position = seek + min;
		return output;
	}
	int newFileBlock(int prev_block_no){
		char block[BLOCK_SIZE];
		int next_block_no = superblock.getFirstFreeBlockNo();
		superblock.first_free_block++;	
		updateSuperBlock();
		string next = addressToString(next_block_no);
		disk.read(prev_block_no,block);
		block[3] = next[0]; block[4] = next[1]; block[5] = next[2]; // updating next address of the previous block
		disk.write(prev_block_no,block);
		stringToCharArray(addressToString(prev_block_no)+"-01",block,' ');				
		disk.write(next_block_no,block);	// creating next block in disk	
		return next_block_no;
	}
	int closeFile(FCB &fcb){
		char block[BLOCK_SIZE];		
		lock_guard<mutex> disk_locker(mutex_disk);
		disk.read(fcb.parent_dir_addr,block);
		Directory dir = Directory(string(block));
		dir.unlockFile(fcb.file_name);
		stringToCharArray(dir.toString(),block,'\0');
		disk.write(fcb.parent_dir_addr,block);
	}
};


/*
int main(){	
	FileSystem fs;
	map<string,FCB> oft;
	fs.initialize();	
	fs.checkUser("rakesh");
	fs.makeDirectory("rakesh","Downloads");
	fs.checkUser("suman");
	fs.makeDirectory("suman","lukkha");
	fs.changeCurrentDirectory("suman","lukkha");
	string file_name = "/root/suman/lukkha/a.txt";
	FCB fcb;
	int ret = fs.openFile("suman",file_name,fcb);
	if(ret == 0){
		oft[file_name] = fcb;
		debug("fcb got: "<< fcb.file_name << fcb.file_addr << fcb.cur_position << fcb.parent_dir_addr);
		fs.fileSeek(fcb,249);
		fs.writeFile(fcb,20,"rakesh");
		fs.fileSeek(fcb,249);
		println(fs.readFile(fcb,20));
		fs.closeFile(fcb);
	}	
	//file_name = "/root/suman/lukkha/b.txt";
}
*/
int toInt(string str){
	if(str[0]=='-'){
		return -1 * stoi(str.substr(1));
	} else {
		return stoi(str);
	}
}
string addressToString(int address){// address to 3 length string
	if(address == -1){
		return "-01";
	} else{
		string ret = to_string(address);
		if(ret.length() == 1){
			return "00" + ret;
		} else if(ret.length() == 2){
			return "0" + ret;
		} else if(ret.length()==3){
			return ret;
		}
		error("invalid address given");
		return "-01";
	}
}
void stringToCharArray(string str, char* ch, char fill_char = '\0'){
	int i;
	for(i = 0; i < str.size() && i < BLOCK_SIZE ; i++){
		ch[i] = str[i];
	}
	while(i < BLOCK_SIZE){
		ch[i] = fill_char;
		i++;
	}
}

// void tokenize(const string& str,
//             vector<string>& tokens,
//             const string& delimiters)
// {
//     // Skip delimiters at beginning.
//     string::size_type lastPos = str.find_first_not_of(delimiters, 0);
//     // Find first "non-delimiter".
//     string::size_type pos     = str.find_first_of(delimiters, lastPos);

//     while (string::npos != pos || string::npos != lastPos)
//     {
//         // Found a token, add it to the vector.
//         tokens.push_back(str.substr(lastPos, pos - lastPos));
//         // Skip delimiters.  Note the "not_of"
//         lastPos = str.find_first_not_of(delimiters, pos);
//         // Find next "non-delimiter"
//         pos = str.find_first_of(delimiters, lastPos);
//     }
// }