#include <cstdlib>
#include <fstream>
#include <iterator>
#include <vector>
#include <sstream>
#include <cctype>
#include <iostream>

#include <stdlib.h>

#include <time.h>

using namespace std;

enum stat_index {
	
	PID, COMM, STATE, PPID, PGRP, SESSION, TTY_NR, TPGID, FLAGS, MINFLT, CMINFLT, 
	MAJFLT, CMAJFLT, UTIME, STIME, CUTIME, CSTIME, PRIORITY, NICE, NUM_THREADS,
	ITREALVALUE, STARTTIME, VSIZE, RSS, RSSLIM, STARTCODE, ENDCODE, STARTSTACK,
	KSTKESP, KSTKEIP, SIGNAL, BLOCKED, SIGIGNORE, SIGCATCH, WCHAN, NSWAP, ONSWAP,
	EXIT_SIGNAL, PROCESSOR, RT_PRIORITY, POLICY, DELAYACCT_BLKIO_TICKS, GUEST_TIME,
	CGUEST_TIME
	
};

/*enum stat_index 
{
    PID, 
    STATE, 
    PPID, 
    UTIME, 
    STIME, 
    CUTIME, 
    CSTIME, 
    PRIORITY, 
    NICE, 
    NUM_THREADS,
    ITREALVALUE, 
    STARTCODE, 
    ENDCODE, 
    KSTKESP, 
    KSTKEIP,
    PROCESSOR, 
};*/
				   
bool check_pid(string s){
	for(int i = 0; i < s.size(); i++){
		char buffer = s[i];
		if(buffer < 48 || buffer > 57){
			return false;
		}
	}
	return true;
}

string set_to_lower(string str){
	for (int i=0; str[i]; i++)
	{
		str[i] = tolower(str[i]);
	}
	return str;
}

/*Local support function from part 1*/
string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

vector<string> &split(const string &s, char delimit_char, vector<string> &elems) {
    stringstream ss(s);
    string elem;
    while (getline(ss, elem, delimit_char)) {
        if (elem.length() > 0) {
            elems.push_back(elem);  
        }
    }
    return elems;
}


vector<string> split(const string &s, char delimit_char) {
    vector<string> elems;
    split(s, delimit_char, elems);
    return elems;
}

int ask_for_pid(){
	cout << "\n******************************\n";
	cout << "To view a list of processes enter 'ls'" << endl;
	cout << "To view currently running processes enter 'ps'" << endl;
	cout << "To clear terminal enter 'clear'" << endl;
	cout << "To terminate enter 'exit'" << endl;
	cout << "******************************\n";
	cout << "Enter process id: ";
	
	string input;
	cin >> input;
	cout << endl;
	input = set_to_lower(input);
	
	if(input == "ls")
	{
		system("ls /proc");
		return ask_for_pid();
	}
	else if(input == "ps")
	{
		system("ps");
		return ask_for_pid();
	}
	else if(input == "exit")
	{
		cout << "GoodBye" << endl;
		return -1;
	}
	else if(input == "clear")
	{
		system("clear");
		return ask_for_pid();
	}
	else if(check_pid(input))
	{
		int pid = atoi(input.c_str());
		cout << "Pid: " << pid << " submitted..." << endl;
		return pid; 
	}
	else
	{
		cout << "ERROR: input is invalid \n";
		return ask_for_pid();
	}
	
}

int main(int argc, char * argv[]) {
	
	int pid = ask_for_pid();
	
	while(pid != -1){
		ifstream stat, status, maps;
		stat.open("/proc/" + int2string(pid) + "/stat");
		status.open("/proc/" + int2string(pid) + "/status");
		
		if(stat && status){
			cout << "\n/proc/" << pid << "/---stat opened---" << endl << endl;
			string raw_inp;
			string buffer;
			while(getline(stat, buffer)){
				raw_inp += buffer;
			}
			cout << "/proc/" << pid << "/stat raw input:" << endl;
			cout << raw_inp << endl << endl;
			istringstream iss(raw_inp);
			istream_iterator<string> beg(iss), end;
			vector<string> stat_vals(beg, end);
			
			cout << "/proc/" << pid << "/---status opened---" << endl << endl;
			raw_inp = "";
			vector<vector<string>> status_vals;
			while(getline(status, buffer)){
				vector<string> split_vect = split(buffer, ':');
				string information = split_vect[1];
				istringstream iss2(information);
				istream_iterator<string> beq(iss2), end;
				vector<string> buffer_vect(beq, end);
				status_vals.push_back(buffer_vect);
				raw_inp += buffer + '\n';
			}
			cout << "/proc/" << pid << "/status raw input:" << endl;
			cout << raw_inp << endl << endl;
			
			
			
			cout << "---Raw input parsed---" << endl << endl;
			
			//Outputting data
			cout <<"*****REQUESTED DATA*****" << endl << endl;
						
			cout << "|CATEGORY 1| Identifiers" << endl; 
			cout << "--Process ID (PID): " << stat_vals.at(PID) << endl;
			cout << "--Parent Process ID (PPID): " << stat_vals.at(PPID) << endl;
			cout << "--Effective User ID (EUID): " << status_vals.at(6).at(1) << endl;
			cout << "--Effective Group ID (EGID): " << status_vals.at(7).at(1) << endl;
			cout << "--Real User ID (RUID): " << status_vals.at(6).at(0) << endl;
			cout << "--Real Group ID (RGID): " << status_vals.at(7).at(0) << endl;
			cout << "--User File System ID (FSUID): " << status_vals.at(6).at(3) << endl;
			cout << "--Group File System ID (FSGID): " << status_vals.at(7).at(3) << endl << endl;
			
			cout << "|CATEGORY 2| State" << endl;
			string state_val = stat_vals.at(STATE);
			cout << "--State (STATE): " << state_val;
			if(state_val == "R")	cout << " Running\n\n";
			else if(state_val == "S")	cout << " (sleeping)\n\n";
			else if(state_val == "D")	cout << " (disk sleeping)\n\n";
			else if(state_val == "T")	cout << " (stopped)\n\n";
			else if(state_val == "Z")	cout << " (zombie)\n\n";
			else if(state_val == "X")	cout << " (dead)\n\n";
			else	cout << "(Unkown)\n\n";
			
			cout << "|CATEGORY 3| Thread Information" << endl;
			cout << "--Thread_Info (NUM_THREADS): " << stat_vals.at(NUM_THREADS) << endl << endl;	
			
			cout << "|CATEGORY 4| Priorty" << endl;
			cout << "--Priority Number (PRIORITY): " << stat_vals.at(PRIORITY) << endl;
			cout << "--Nice Value (NICE): " << stat_vals.at(NICE) << endl << endl;
			
			cout << "|CATEGORY 5| Time Information" << endl;
			cout << "--Time in kernel mode (STIME): " << stat_vals.at(STIME) << endl;
			cout << "--Time in user mode (UTIME): " << stat_vals.at(UTIME) << endl;
			cout << "--Time waiting on children in kernel mode (CSTIME): " << stat_vals.at(CSTIME) << endl;
			cout << "--Time waiting on children in user mode (CUTIME): " << stat_vals.at(CUTIME) << endl << endl;
			
			cout << "|CATEGORY 6| Address Space" << endl;
			cout << "--Startcode (STARTCODE): " << stat_vals.at(STARTCODE) << endl;
			cout << "--Endcode (ENDCODE): " << stat_vals.at(ENDCODE) << endl;
			cout << "--ESP (KSTKESP): " << stat_vals.at(KSTKESP) << endl;
			cout << "--EIP (KSTKEIP): " << stat_vals.at(KSTKEIP) << endl << endl;
			
			cout << "|CATEGORY 7| Resources" << endl;
			cout << "--Number of file descriptors (FDSize): " << status_vals.at(8).at(0) << endl;
			cout << "--Number of voluntary context switches (voluntary_ctxt_switches): " << status_vals.at(37).at(0) << endl;
			cout << "--Number of nonvoluntary context switches (nonvoluntary_ctxt_switches): " << status_vals.at(38).at(0) << endl << endl;
			
			cout << "|CATEGORY 8| Processors" << endl;
			cout << "--Allowed processors (Cpus_allowed): " << status_vals.at(33).at(0) << endl;
			cout << "--Last executed processor (PROCESSOR): " << stat_vals.at(PROCESSOR)  << endl << endl;
			
		}else{
			cout << "ERROR: pid " << pid << " could not be opened" << endl;
			cout << "Note: This process may not exist, enter 'list' to find available processes" << endl;
		}
		
		cout << "\n--Accessing memory map--" << endl << endl;
		maps.open("/proc/" + int2string(pid) + "/maps");
		if(maps){
			cout << "--Memory map accessed--" << endl << endl;
			string buffer;
			string raw_inp = "";
			int i = 0;
			while(getline(maps, buffer) && i < 3){
				raw_inp += buffer + '\n';
				i++;
			}
			if(raw_inp == ""){
				cout << "--Memory map: empty--\n";
			}else{
				cout << "|CATEGORY 9| Memory Map" << endl;
				cout << raw_inp << endl << endl;
				ofstream out;
				out.open("out.txt");
				out << raw_inp;
				out.close();
			}				
		}else{
			cout << "ERROR: cannot access memory map (check if running)\n";
		}
		
		stat.close();
		status.close();
		maps.close();
		pid = ask_for_pid();
	}
	
	cout << "And have a great day!" << endl;
	return 0;
}