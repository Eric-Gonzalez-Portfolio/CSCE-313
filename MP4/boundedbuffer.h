
#include <pthread.h>
#include <queue>
#include <string>
#include "semaphore.h"
#include <vector>

using namespace std;

//ThreadInformation class, to contain the data, thread_id, and request counter
class ThreadInformation
{
  public:

    string data_contents;  
    int thread_id;  
    int req_count;       


    ThreadInformation(string c, int tid, int rc) 
	{
      data_contents = c;
      thread_id = tid;
      req_count = rc;
    }
};

//Bounded buffer class
class BoundedBuffer 
{
  private:
	Semaphore * empty;
    Semaphore * full;
	Semaphore * mutex;
    vector<ThreadInformation> data;

  public:
  //constructor
   BoundedBuffer(int b)
   {
		empty = new Semaphore(b);
		full = new Semaphore(0);
		mutex = new Semaphore(1);
	}
	//destructor
~BoundedBuffer()
	{
		delete mutex;
		delete full;
		delete empty;
	}
//add 
//adds item to the vector	
void add(ThreadInformation item)
	{
		empty->P();
		mutex->P();
		data.push_back(item); 
		mutex->V();
		full->V();
	}
//remove 
//gets next, returns it, and deletes from list	
ThreadInformation remove()
	{
		full->P();
		mutex->P();
		ThreadInformation t = data[0];
		data.erase(data.begin());
		mutex->V();
		empty->V();
		return t;
	}

};

