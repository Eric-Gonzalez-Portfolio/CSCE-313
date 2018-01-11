#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "linked_list2.h"

/*
Run on linux server with:
gcc *.c -o testlist2
testlist2 -b 128 -s 512 -t 4
*/

int main(int argc, char ** argv) 
{
	
	//Rewrote sample main for part 2 to achieve program requirements
	
	//Default Values
	int b = 128;
	int M = b * 16;  	// so we have space for 16 items in the whole list
	int t = 4;			// 4 tiers and 4 items per tier
	
	int c;
	int r;
	int maxnodes;
	int val = 2147483647; //2^31-1 signed values = 2,147,483,647
	
	char *bChar;
	char *sChar;
	char *tChar;
	extern char *optarg;

	while ((c = getopt(argc, argv, "b:s:t:")) != -1)
		switch (c) {
			case 'b':
				bChar = optarg;
				b = atoi(bChar);
				break;
			case 's':
				sChar = optarg;
				M = atoi(sChar);
				break;
			case 't':
				tChar = optarg;
				t = atoi(tChar);
				break;
		}

		Init (M, b, t); // initialize tiers of memory pools

		maxnodes = M/b;

		printf("\n----- \nFilling a Tiered Memory Pool \nMemory Size: %d\nBlock Size: %d\nTiers: %d\nInserting %d elements\n-----\n\n",M,b,t,maxnodes);

		srand(time(NULL)); //Initialize random seed

		//Fills the tiered memory pool with random numbers.
		int i=0;
		for (i=0; i< maxnodes; i ++)
		{
			r = rand() % val ; //Random signed value
			Insert (r, "test", 50);
		}

		PrintList ();

		//Functions Testing

		// Inserting element
		printf("\n----- \nInserting Element with Key: 1 \n-----\n");
		Insert (1, "test", 50);

		PrintList ();

		// Looking for an existing element
		printf("\n----- \nLooking for an existing element (Key: 1) \n-----\n");
		char* kv = Lookup (1);
		if (kv) {
			printf("\n----- \n Element Found \n----- \n");
			printf ("\n Key = %d, Value Len = %d, Value = %s\n\n", *(int *) kv, *(int *) (kv+4), kv + 8);
		} else {
			printf("\n----- \n Element NOT Found \n----- \n");
		}
			
		// Deleting existing element
		printf("\n----- \nDeleting an existing element (Key: 1) \n-----\n");
		Delete (1);

		PrintList ();

		// Looking for non-existent element - this will fail and will not print anything
		printf("\n----- \nLooking for a non existent element (Key: 5) \n-----\n");
		kv = Lookup (5);
		if (kv) {
			printf("\n----- \n Element Found \n----- \n");
			printf ("\nKey = %d, Value Len = %d, Value = %s\n\n", *(int *) kv, *(int *) (kv+4), kv + 8);
		} else {
			printf("\n----- \n Element NOT Found \n----- \n");
		}

		// Deleting a non-existent element
		printf("\n----- \nDeleting a non existent element (Key: 5) \n-----\n");
		Delete (5);

		//Free Tiered Memory Pool
		printf("\n----- \nFreeing Tiered Memory Pool\n-----\n");
		Destroy();
	
}