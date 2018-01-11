/* 
    File: my_allocator.c

    Author: <your name>
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified: 

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<math.h>
#include "my_allocator.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */
	
typedef struct Header Header;

    struct Header{
	int size;
    char symbol;
    Header* next;
    };

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

Header** listPtr;
double listSize;
double base;
uintptr_t memoryStart;	//pointer to start of memory block for bit calculation

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/


int my_header_check(Header* ptr)
{

    if (ptr->symbol == '~')  //free
    {
        //  printf("free memory of size %d\n",ptr->size);
        return 1;
    }
    if (ptr->symbol == '!')  //used
    {
        // printf("used memory of size %d\n", ptr->size);
        return 0;
    }
    return -1;


}

void my_header_remove(int i)
{
    if (listPtr[i] != listPtr[i+1])
        listPtr[i] = listPtr[i]->next;
    else
    {
        listPtr[i] = NULL;
        listPtr[i+1] = NULL;
    }
}

void test_functionality()  //testing
{

    for (int i = 0; i < listSize*2; i+=2)
    {
        Header* ptr = listPtr[i];
        printf("\n***Free List: %d\n", i/2);
        int sum = 0;
        while (ptr != NULL)
        {
            int temp = ptr->size;
            sum += temp;
            printf("Size of memory block: %d\n", temp);
            if (ptr == listPtr[i+1])
                break;
            ptr = ptr->next;
        }
        printf("Total memory of list: %d\n", sum);
    }
}

int power_of(int num, int pow)  //exponent function
{
    if (pow == 0)
        return 1;
    else
        num = num*power_of(num,pow-1);
	
    return num;
}

void join_block(Header* ptr)
{
    uintptr_t temp_ptr = (uintptr_t)ptr;	//cast pointer to int for bit operations
    temp_ptr = temp_ptr - memoryStart;
    int i = 0;
    i = log2(ptr->size);
    temp_ptr ^= 1 << i ;		//toggle log base 2 of size bit from the right
    temp_ptr = temp_ptr + memoryStart;
    Header* buddy = (Header*)temp_ptr; 		//cast back from int to Header ptr for operations

    if (my_header_check(buddy) == 1 && buddy->size == ptr->size)
    {
        if((int)(buddy - ptr) < 0)
            ptr = buddy;
        my_header_remove((listSize - 1 - (log2(ptr->size) - log2(base)))*2);
        ptr->size = (ptr->size*2);
        join_block(ptr);
    }

}

Header* break_block(int i) // split/break blocks
{
    unsigned int break_block_size = base*power_of(2,(listSize - 1 - (i/2)));		//block size to end with
    // printf("breaking block size to index i = %d\n", i);
    Header* head;
    i -= 2;		//increases index to next size; decrease because of array holding largest block size first

    if (i <= listSize*2 && i >= 0)
    {
        if (listPtr[i] == NULL) 	//if no blocks split next size
            if (break_block(i) == NULL)
                return NULL;
        head = listPtr[i];

        my_header_remove(i);
        i += 2;
        head->size = break_block_size;	//change old header block size
        head[break_block_size/sizeof(Header)] = head[0];
        head->next = &head[break_block_size/sizeof(Header)];
        listPtr[i] = head;
        head = head->next;
        head->next = NULL;
        listPtr[i+1] = head;
        my_header_check(head);
        return head;
    }
    else
        return NULL;
}


/* Don't forget to implement "init_allocator" and "release_allocator"! */



unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length)
{
    unsigned int remainder = _length % _basic_block_size;
    unsigned int total = _length - remainder; //take away memory that will not fit into multiple of basic block size
    if (remainder != 0)
        total = total + _basic_block_size; //increment to allocate an extra block for left over bytes
    _length = total;
    listSize = log2(total) - log2(_basic_block_size); //determine number of lists
    listSize = round(listSize + .5);	//add .5 for rounding up
    base = _basic_block_size;
    Header** free_list = (Header**)malloc(2*listSize*sizeof(Header*)); //free list array

    free_list[0] = (Header*)malloc(total); //have os get continuous chunk of memory and store in free_list
    memoryStart = (uintptr_t)free_list[0];
    for (int i = 1; i < listSize*2; ++i)
        free_list[i] = NULL; //initalize pointers to Null
    Header* ptr = free_list[0]; //set pointer that will be used to create headers to beginning of list
    Header head;
    head.symbol = '~';
    head.next = NULL;
   // printf("%f list size for the memory\n",listSize);
    for (int i = 0; i < listSize*2; i += 2)
    {
        head.size = (_basic_block_size*power_of(2,(listSize - ((i+2)/2))));
     //   printf("\n%d working iteration %d  power_of is %d\n",total, i/2,power_of(2,(listSize-(((i+2)/2)))));
        if (total >= (_basic_block_size*power_of(2,(listSize - (((i+2)/2)))))) //so that the first pointer only gets initialized once
            free_list[i]=ptr; //else ptr is equal to null
        while (total >= (_basic_block_size*power_of(2,(listSize - (((i+2)/2))))))
        {

            ptr[0] = head;
            ptr->next = &ptr[(_basic_block_size*power_of(2,(listSize - (((i+2)/2)))))/sizeof(Header)];
            ptr = ptr->next;
            total = total - _basic_block_size*power_of(2,(listSize - (((i+2)/2))));
            //printf("total mem unallocated %d\n",total);
            if (ptr->size == free_list[i]->size)
                free_list[i+1] = ptr;
            else
                free_list[i+1] = free_list[i];
        }
    }

    listPtr = free_list;

    return _length; 	//return amount of memory made available
}

int release_allocator()
{
    /* This function returns any allocated memory to the operating system.
       After this function is called, any allocation fails.
    */
    free((void*)listPtr);
    free((void*)memoryStart);
    printf("deallocated memory\n");
    return 0;
}

extern Addr my_malloc(unsigned int _length)
{
    /* This preliminary implementation simply hands the call over the
       the C standard library!
       Of course this needs to be replaced by your implementation.
    */
    // determine block index/size needed (remember all blocks have a header)
    printf("allocating %d\n",_length);
    int index = 0;
    while ((_length + sizeof(Header))/(base*power_of(2,index)) > 1)
        ++index;
    if(index >= listSize)
    {
        printf("my_malloc ERROR: Request exceeds biggest memory block\n");
        return NULL;
    }

    index = listSize - 1 - index;			//invert index
    if (listPtr[index*2] == NULL){
        printf("breaking blocks to listPtr; i = %d\n",index*2);
        if (break_block(index*2) == NULL) 	//break blocks if unsuccessful
        {
            printf("ERROR: block break unsuccessful--no memory left--returning NULL\n");
            return (void*)NULL;
        }
    }
    if (my_header_check(listPtr[index*2]) == 0)
        printf("my_malloc ERROR: Acessing used memory\n");
    if (my_header_check(listPtr[index*2]) == -1)
        printf("my_malloc ERROR: Invalid header symbol\n");

    listPtr[index*2]->symbol = '!'; 		//symbol signifies used memory
    Addr address = &listPtr[index*2][1];

    my_header_remove(index*2);
	//return malloc((size_t)_length);
    return address;
}

extern int my_free(Addr _a)
{
	/* Same here! */
    Header* free_addr = (Header*)_a;
    int* eraser;
    eraser = (int*)free_addr;	//pointer for erasing memory

    free_addr = &free_addr[-1]; //shift pointer: used mem to header

    if (my_header_check(free_addr) == 1)
        printf("my_free ERROR: Block already free!\n");
    if (my_header_check(free_addr) == -1)
        printf("my_free ERROR: Not a header!\n");
    free_addr->symbol = '~';	//set symbol to free
    join_block(free_addr);
    int i = 0;
    while((free_addr->size - sizeof(Header)) > (i*sizeof(int)))
    {
        eraser[i] &= 0;
        ++i;
    }
    i = 0;

    while (free_addr->size > (base*power_of(2,i))) //find index
        ++i;
    i = listSize - 1 - i;		//invert index for accessing list

    if (listPtr[i*2] == NULL) 	//add to list
    {
        listPtr[i*2] = free_addr;
        listPtr[i*2+1] = free_addr;
    }
    else
    {
        free_addr->next = listPtr[i*2];
        listPtr[i*2] = free_addr;
    }


    //free(_a);
    return 0;
}
