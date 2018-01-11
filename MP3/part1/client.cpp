/* 
    File: simpleclient.cpp

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2012/07/11

    Simple client main program for MP2 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <cmath>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <chrono>

#include <errno.h>
#include <unistd.h>

#include "reqchannel.h"

using namespace std;
using namespace std::chrono;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

int cwrite_localized(string _msg) {
    
    if (_msg.length() >= 255) {
        cerr << "ERROR: message too long for channel\n";
        return -1;
    }
    
    const char * s = _msg.c_str();
    
    return 1;
}

string int2string(int number) {
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}


void process_hello_localized(RequestChannel & _channel, const string & _request) {
    cout << "LOCAL: hello to you too" << endl;
}

void process_data_localized(RequestChannel & _channel, const string &  _request) {
    cwrite_localized(int2string(rand() % 100));
}


void process_request_localized(RequestChannel & _channel, const string & _request) {
    
    if (_request.compare(0, 5, "hello") == 0) {
        process_hello_localized(_channel, _request);
    }
    else if (_request.compare(0, 4, "data") == 0) {
        process_data_localized(_channel, _request);
    }
    else {
        cwrite_localized("unknown request");
    }
    
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

    int pid = fork();
	
    if (pid == 0) {
        //child process, run dataserver
        system("./dataserver");
    } 
	
	else {
        int num_of_requests = 10000;
        double request_times[num_of_requests];
        double local_request_times[num_of_requests];
        
        cout << "CLIENT STARTED:" << endl;

        cout << "Establishing control channel... " << flush;
        RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
        cout << "done." << endl;
        
        /* Execution time variables */
        high_resolution_clock::time_point pre_request_time,         // start of server request
                                          post_request_time,        // end of server request
                                          pre_local_request_time,   // start of local request
                                          post_local_request_time;  // end of local request
        double runtime;
        double local_runtime;

        /* -- Start sending a sequence of requests */

        string reply1 = chan.send_request("hello");
        cout << "Reply to request 'hello' is '" << reply1 << "'" << endl;

        string reply2 = chan.send_request("data Joe Smith");
        cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << endl;

        string reply3 = chan.send_request("data Jane Smith");
        cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << endl;

        for(int i = 0; i < num_of_requests; i++) {
            string request_string("data TestPerson" + int2string(i));
            
            // Send request to dataserver
            pre_request_time = high_resolution_clock::now();
            string reply_string = chan.send_request(request_string);
            post_request_time = high_resolution_clock::now();
            
            // Capture execution time from dataserver
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>( post_request_time - pre_request_time ).count();
            runtime += duration;
            request_times[i] = duration;
            
            // Print details of request response
            cout << "Reply to request took " << duration << " microseconds. Current total time: " << i << ":" << reply_string << endl;;
            
            // Handle request locally
            pre_local_request_time = high_resolution_clock::now();
            process_request_localized(chan, request_string);
            post_local_request_time = high_resolution_clock::now();
            
            // Calculate duration time to compare with server request
            auto local_duration = std::chrono::duration_cast<std::chrono::microseconds>( post_local_request_time - pre_local_request_time ).count();
            local_runtime += local_duration;
            local_request_times[i] = local_duration;
        }
        
        string reply4 = chan.send_request("quit");
        cout << "Reply to request 'quit' is '" << reply4 << endl;

        usleep(1000000);
        
		// Calculate time for local requests
        long double local_avg_time = 0.0;
        for(int i = 0; i < num_of_requests; i++){
            local_avg_time += local_request_times[i];
        }
        local_avg_time = local_avg_time/num_of_requests;
        
        long double local_stand_dev = 0.0;
        for(int i = 0; i < num_of_requests; i++){
            local_stand_dev += ( (local_request_times[i] - local_avg_time)*(local_request_times[i] - local_avg_time) );
        }
        local_stand_dev = sqrt( local_stand_dev/(num_of_requests - 1) );
        
        // Print local request
        cout << " ******************************************** " << endl << flush
             << "                Local Results               " << endl << flush
             << "********************************************" << endl << flush
             << " runtime           |" << setw(20) << local_runtime/1000 << " ms " << endl << flush
             << " # of requests     |" << setw(23) << num_of_requests << " " << endl << flush
             << " avg. request time |" << setw(23) << local_avg_time << " " << endl << flush
             << " standard dev.     |" << setw(23) << local_stand_dev << " " << endl << flush
             << " ******************************************** " << endl << flush;
		
        // Calculate dataserver time info
        long double avg_time = 0.0;
        for(int i = 0; i < num_of_requests; i++){
            avg_time += request_times[i];
        }
		
        avg_time = avg_time/num_of_requests;
        
        long double stand_dev = 0.0;
        for(int i = 0; i < num_of_requests; i++){
            stand_dev += ((request_times[i]-avg_time)*(request_times[i]-avg_time));
        }
		
        stand_dev = sqrt(stand_dev/(num_of_requests-1));
        
        // Print server request
        cout << " ******************************************** " << endl << flush
             << "                DataServer Results              " << endl << flush
             << "********************************************" << endl << flush
             << " runtime           |" << setw(20) << runtime/1000 << " ms " << endl << flush
             << " # of requests     |" << setw(23) << num_of_requests << " " << endl << flush
             << " avg. request time |" << setw(23) << avg_time << " " << endl << flush
             << " standard dev.     |" << setw(23) << stand_dev << " " << endl << flush
             << " ******************************************** " << endl << flush;
        
        // Calculate time for local requests
        /*long double local_avg_time = 0.0;
        for(int i = 0; i < num_of_requests; i++){
            local_avg_time += local_request_times[i];
        }
        local_avg_time = local_avg_time/num_of_requests;
        
        long double local_stand_dev = 0.0;
        for(int i = 0; i < num_of_requests; i++){
            local_stand_dev += ( (local_request_times[i] - local_avg_time)*(local_request_times[i] - local_avg_time) );
        }
        local_stand_dev = sqrt( local_stand_dev/(num_of_requests - 1) );
        
        // Print local request
        cout << " ******************************************** " << endl << flush
             << "                Local Results               " << endl << flush
             << "********************************************" << endl << flush
             << " runtime           |" << setw(20) << local_runtime/1000 << " ms " << endl << flush
             << " # of requests     |" << setw(23) << num_of_requests << " " << endl << flush
             << " avg. request time |" << setw(23) << local_avg_time << " " << endl << flush
             << " standard dev.     |" << setw(23) << local_stand_dev << " " << endl << flush
             << " ******************************************** " << endl << flush;*/
    }
}
