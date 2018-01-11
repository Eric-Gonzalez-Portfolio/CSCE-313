#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv){
	int b = 128;

	int M = b * 12;  // so we have space for 12 items

	int c;
	
	char *bChar;
	char *sChar;
	
	extern char *optarg;
    
	//take standard input
	while ((c = getopt(argc, argv, "b:s:")) != -1){
		switch (c){		//set b and M
			case 'b':
				bChar = optarg;
				b = atoi(bChar);
				break;
			case 's':
				sChar = optarg;
				M = atoi(sChar);
				break;
		}
	}

	char buf [1024];

	memset (buf, 1, 1024); // set each byte to 1

	char * msg = "a sample message";

	Init (M,b); // initialize

	// test operations

	int testnums [] = {100, 5, 200, 7, 39, 25, 400, 50, 200, 300};

	int i = 0;

	// some sample insertions

	for (i=0; i< 10; i ++)

	{ 

	Insert (testnums [i], buf, 50);   // insert 50 bytes from the buffer as value for each of the insertions

	}

	PrintList ();

	Insert (150, buf, 200); // this Insert should fail

	Delete (200);

	Delete (7);

	PrintList ();

	Insert (13, msg, strlen(msg)+1); // insertion of strings, copies the null byte at the end

	Delete (100);

	Delete (300);

	PrintList ();

	Insert (500, msg, strlen(msg)+1);

	Insert (600, msg, strlen(msg)+1);

	PrintList ();

	// a sample lookup operations that should return null, because it is looking up a non-existent number

	int _key = 3;

	char* kv = Lookup (_key);

	if (kv) 

	printf ("Key = %d, Value Len = %d\n", *(int *) kv, *(int *) (kv+4));

	else 

	printf ("Key = %d could not be found\n", _key);

	// this look up  should succeed and print the string "a sample message"

	_key = 13;

	kv = Lookup (_key);

	if (kv) 

	printf ("Key = %d, Value Len = %d, Value = %s\n", *(int *) kv, *(int *) 

	(kv+4), kv + 8);

	else 

	printf ("Key = %d could not be found\n", _key);

	// end test operations 

	Destroy ();
}