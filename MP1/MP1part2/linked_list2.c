#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list2.h"

int node_len;
int num_nodes;
int tiers, tier_size; //tier variables
int val = 2147483647; //2^31-1 signed values

struct node *list_head;  //list's head pointer
struct node *aop[16];

struct node {
	struct node *next;
    int k;
    int len;

};

/*Initialize the list and tiers of mem pool*/
void 	Init (int M, int b, int t){
    //Create t memory pools
    int i;

    struct node *temp; //temporary pointer to head of new tier memory pool

    for (i=0;i<t;i++)
    {
        //Create new memory pool and assign it to the next available pointer
        temp = (struct node *) malloc(M*sizeof(*list_head));
        aop[i] = temp;
        temp=NULL;

    }

    node_len = b;    //sets each node to b bytes
    num_nodes = (M*t)/b; //total number of nodes

    //set tier variables from Init function
    tiers = t;
    tier_size = val/t;

} 

/*Free up memory taken by the tiers of memory pools*/
void 	Destroy (){
    int i;
    for (i=0;i<16;i++)
    {
        free(aop[i]);
    }
    
} 

// Helper function to get the tier the key is located at
struct node * findTier(int k){
    int t = 0;
	int tHash = 0;
    int lMin = 0;
    int lMax = tier_size;

    //Go through the tiers to find where the key is located
    for (t=0; t < tiers; t++)
    {
        if (k > lMin && k < lMax){
            tHash=t;

            int i;
            for (i = 0; i < tiers; i++){
                if (i == tHash){
                    return &aop[i];
                }
            }
            
        } else {
            lMin = lMax;
            lMax += tier_size;
        }
    }

}

//Insert a node
int 	Insert (int key, char * value_ptr, int value_len){
    struct node *iterator;

    //Grab the header pointer to the tier the key will be deleted from.
    iterator = findTier(key);
    
    if (num_nodes == 0) {
        printf("List is full\n");
        return 0;
    }

    /*Goes through list until it reaches the end*/
    if (iterator != 0) {
        while (iterator->next != 0) {
            iterator = iterator->next;
        }
    }
    
    /*Check value's length--see if it fits in node*/
    if(16 + value_len > node_len) {
        printf("String too long for node\n");
        return 0;
    } 

    iterator->next = (char *) iterator + node_len;
    iterator = iterator->next;    //set iterator to new end node
    iterator->next = 0;            //set new end node pointer to 0
    iterator->k = key;             //set node's key
    iterator->len = value_len;  //set node's value length
    memcpy((char *) iterator + 16, value_ptr, value_len); 
    num_nodes--;

}

/*Search for key, then delete it*/
int 	Delete (int key){
    struct node *iterator;
    
    /*Grab the header pointer to the tier the key will be deleted from*/
    iterator = findTier(key);
    
    /*Check list_head for key before traversing*/
    if(iterator->k == key) {
        list_head = (char *) list_head + node_len;
        return 0;
    }

    /*Go through list looking for key*/
    while (iterator->next != 0) { 
        iterator = iterator->next;
        if(iterator->k == key) { //if found, makes prev node pointer point two nodes ahead
            iterator = (char *) iterator - node_len;
            iterator->next = (char *) iterator + (2*node_len);
            return 0;
        }
    }
    printf("\n----- \n Cannot delete %d as it is not in the list\n-----\n", key); //Key is not found.
}

/*Go through list for key*/
char* 	Lookup (int key){
    struct node *iterator;

    /*Grab the header pointer to the tier the key will be deleted from*/
    iterator = findTier(key);

    if(iterator->k == key) {
        return (char *) iterator + 8; //Grabs the header pointer to the tier the key will be deleted from.
    }
    while (iterator->next != 0) { 
        iterator = iterator->next;
        if(iterator->k == key) {
            return (char *) iterator + 8;
        }
    }
    return NULL;
}

/*Go through list and print elements*/
void 	PrintList (){
    struct node *iterator;
    
    int i;

    //Traverse through all the elements on all the tiers and print
    for (i=0;i<tiers;i++){

        iterator = aop[i];

        printf("Tier #%d... \n",(i+1));
        printf("Printed List: \n");
        while (iterator->next != 0) {
            iterator = iterator->next;
            printf("Key = %d // ", iterator->k);
            printf("Value Length = %d // ", iterator->len);
            printf("Message: \"%s\"\n\n", (char *) iterator + 16);
        }
        printf("\n");

    }

}