#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <ctype.h>
#include "ackerman.h"
#include "my_allocator.h"

int main(int argc, char ** argv) {

  // input parameters (basic block size, memory length)
  //Default Values
	int b = 128;
	int m = 5242880;  // default of 512kB
	
	int c;
	
	char *bChar;
	char *sChar;
	
	extern char *optarg;
    
	//get the standard input
	while ((c = getopt(argc, argv, "b:s:")) != -1){
		switch (c){		//set b and m
			case 'b':
				bChar = optarg;
				b = atoi(bChar);
				break;
			case 's':
				sChar = optarg;
				m = atoi(sChar);
				break;
		}
	}

  // init_allocator(basic block size, memory length)
  init_allocator(b,m);
  
  //test_functionality();
  
  ackerman_main();

  // release_allocator()
  atexit((void(*)())release_allocator);

}
