#include <stdio.h>
#include "linked_list.h"

int node_len;
int num_nodes;
struct node *list_head;  //list's head pointer

struct node {
    int len;
	int k;
	struct node *next;
};

/*Initialize the list*/
void 	Init (int M, int b){
    list_head = (struct node *) malloc(M*sizeof(*list_head)); //allocate memory
    list_head->next = 0;
    node_len = b;        //set each node to b bytes
    num_nodes = M/b;  //global variable for max # nodes
} 

/*Free up memory taken by list*/
void 	Destroy (){
    free(list_head);
} 	

/*Insert node*/
int 	Insert (int key, char * value_ptr, int value_len){
    struct node *iterator;
    iterator = list_head;
    
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
    iterator = list_head;
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
    printf("\n----- \n Cannot delete %d as it is not in the list \n-----\n", key); //Key not found.
}

/*Go through list for key*/
char* 	Lookup (int key){
    struct node *iterator;
    iterator = list_head;
    if(iterator->k == key) {
        return (char *) iterator + 8;
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
    iterator = list_head;
    printf("Printed List: \n");
    while (iterator->next != 0) {
            iterator = iterator->next;
            printf("Key = %d | ", iterator->k);
            printf("Value Length = %d | ", iterator->len);
            printf("Message: \"%s\"\n\n", (char *) iterator + 16);
    }
    printf("\n");
}