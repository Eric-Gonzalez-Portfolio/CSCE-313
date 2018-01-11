/* 
    File: simpleclient.C

    Author: R. Bettati
            Department of Computer Science
            Texas A&M University
    Date  : 2013/01/31

    Simple client main program for MP3 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/


#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <stdio.h>

#include <time.h>
#include <sys/time.h>

#include "reqchannel.h"
#include "boundedbuffer.h"
#include <unistd.h>
#include <iomanip>
#include <stdlib.h> 
#include <pthread.h>

using namespace std;

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

   

/*--------------------------------------------------------------------------*/
/* declarations */
/*--------------------------------------------------------------------------*/
	
	int n; //number of data requests per person
    int w; //number of worker threads
    int b; //size of bounded buffer

	//counts number of requests for each person
    int joe_count = 0; 
	int john_count = 0;
    int jane_count = 0;
	
    BoundedBuffer* buffer; 
	//statistics buffers
    BoundedBuffer* joe_buff; 
	BoundedBuffer* john_buff;
    BoundedBuffer* jane_buff;
    

	//counts number of requests for each person
    //int joe_count=0; 
	//int john_count=0;
    //int jane_count=0;

	//stores frequency of data values
    vector<int> joe_histogram(100);  
	vector<int> john_histogram(100);
    vector<int> jane_histogram(100);
    
	//pointers for pthread_create args
	int* joe = new int(0);
	int* john = new int(1);
	int* jane = new int(2);
	
	timeval begin, end; //timer stuff

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* other functions */
/*--------------------------------------------------------------------------*/


//request thread
void* request_thread(void* request_id) {
    int person_id = *((int*)request_id);
  	for(int i=0; i<n; i++){
  		ThreadInformation* t = new ThreadInformation(" ", person_id, 0);
  	  	if (person_id == 0) {
        	joe_count++;
        	t->data_contents = "data Joe Smith";
        	t->thread_id = 0;
        	t->req_count = joe_count;
  	  	}
  	  	else if (person_id == 1){
        	john_count++;
        	t->data_contents = "data John Doe";
        	t->thread_id = 1;
        	t->req_count = john_count;
  	  	}
  	  	else if (person_id == 2){
        	jane_count++;
        	t->data_contents = "data Jane Smith";
        	t->thread_id = 2;
        	t->req_count = jane_count;
  	  	}
  	  	buffer->add(*t);
  	  	delete t;
  }
  if (person_id == 0){
  	  cout << "Joe request threads complete" << endl;
  }
  else if (person_id == 1){
	  cout << "John request threads complete" << endl;
  }
  else if (person_id == 2){
  	  cout << "Jane request threads complete" << endl;
  }
  else
	  {cout << "We're not alone..." << endl;}
  
}


//worker thread
void* worker_thread(void* rc) {
    RequestChannel * channel = (RequestChannel*) rc;
  	ThreadInformation t(" ", 0, 0);
    //int counter = 0;
	
  	while(true) {
		t = buffer->remove(); 
    	if (t.data_contents == "kill") {break;}
    	string reply = channel->send_request(t.data_contents);
    	t.data_contents = reply;
    	if (t.thread_id == 0)
        	{joe_buff->add(t);}
    	else if (t.thread_id == 1)
        	{john_buff->add(t);}
    	else if (t.thread_id == 2)
        	{jane_buff->add(t);}
  	}
	
  	channel->send_request("quit");
}


//statistics thread
void* statistics_thread(void* request_id) {
    int person_id = *((int*)request_id);
  	ThreadInformation t(" ", -1, -1);
	
  	for(int i=0; i<n; i++) {
		
		if (person_id == 0){
			t = joe_buff->remove();
			int value = atoi(t.data_contents.c_str());
        	joe_histogram[value] += 1; //increment frequency for the histogram
    	}
		
		else if (person_id == 1){
            t = john_buff->remove();
			int value = atoi(t.data_contents.c_str());
            john_histogram[value] += 1;
    	}
		
		else if (person_id == 2){
			t = jane_buff->remove();
			int value = atoi(t.data_contents.c_str());
            jane_histogram[value] += 1;
    	}
	}
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

    int option;
	int def = 0;
    while((option = getopt (argc, argv, "n:w:b:"))!=-1 )
    switch(option) {
      case 'n':
	    def++;
        n = atoi(optarg);
        break;
      case 'w':
	    def++;
        w = atoi(optarg);
        break;
      case 'b':
	    def++;
        b = atoi(optarg);
        break;
    }
	
	if (def != 3)
	{
		cout << "NO VALUES ENTERED, DEFAULT VALUES USED" << endl;
		n = 10000;
		w = 40;
		b = 100;
	}
	
	cout << "n = " << n << " w = " << w << " b = " << b << endl;
	
	if (fork()) {
		// 0=joe 1=john 2=jane
		pthread_t req_threads[3];  
		pthread_t worker_threads[w];
		pthread_t stat_threads[3];

		buffer = new BoundedBuffer(b);
		joe_buff = new BoundedBuffer(b);
		john_buff = new BoundedBuffer(b);
		jane_buff = new BoundedBuffer(b);

		cout << "CLIENT STARTED:" << endl;

		cout << "Establishing control channel... " << flush;
		RequestChannel chan("control", RequestChannel::CLIENT_SIDE);
		cout << "done." << endl;

		
		gettimeofday(&begin, NULL);//start timer
	 
		pthread_create(&req_threads[0], NULL, request_thread, (void*)joe);
		pthread_create(&req_threads[1], NULL, request_thread, (void*)john);
		pthread_create(&req_threads[2], NULL, request_thread, (void*)jane);

		for(int i=0; i<w; i++) {
			string reply = chan.send_request("newthread");
			RequestChannel* rc = new RequestChannel(reply, RequestChannel::CLIENT_SIDE);
			pthread_create(&worker_threads[i], NULL, worker_thread, rc);
		}

		pthread_create(&stat_threads[0], NULL, statistics_thread, (void*)joe);
		pthread_create(&stat_threads[1], NULL, statistics_thread, (void*)john);
		pthread_create(&stat_threads[2], NULL, statistics_thread, (void*)jane);

		pthread_join(req_threads[0], NULL);
		pthread_join(req_threads[1], NULL);
		pthread_join(req_threads[2], NULL);

		ThreadInformation k("kill", -1, -1); 
		cout<<"Deallocating worker threads"<< endl;
		for (int i=0; i<w; ++i)
			{buffer->add(k);}
		for (int i=0; i<w; ++i) 
			{pthread_join(worker_threads[i], NULL);}
		
		pthread_join(stat_threads[0], NULL); 
		pthread_join(stat_threads[1], NULL);
		pthread_join(stat_threads[2], NULL);

		gettimeofday(&end, NULL); //stop timer

		chan.send_request("quit");
		cout << "                     HISTOGRAM" << endl;
		cout << "Value   |	 Joe 	   |    John	   |  Jane" << endl;
		cout << "****************************************************" << endl;
		for (int i=0; i<100; i++){
			 cout << i << " 	|	 " << joe_histogram[i] << "	   |    " << john_histogram[i] << "	   |   " << jane_histogram[i] << endl;
		}
		cout << "Total requests: " << n*3 << endl;
		cout << "Number of worker threads: " << w << endl;
		cout << "Size of bounded buffer: " << b << endl;
		int micro_dif = end.tv_usec-begin.tv_usec;
		int sec_dif = end.tv_sec-begin.tv_sec;
		if (micro_dif < 0)
		{
			micro_dif *= -1;
		}
		cout << "Total time: " << sec_dif << " seconds and "<< micro_dif <<" micro seconds" << endl;
	}
	else
	{
		execl("dataserver", "", (char*)NULL);
	}
}
