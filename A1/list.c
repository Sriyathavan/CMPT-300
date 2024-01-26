#include "list.h"
#include <stdio.h>


//static variables
static int uninitialized = 1, activeHeads = 0;

//hold the location of start and end (or length)
static void* startLocation;
static void* endLocation;

//empty nodes
static List* emptyNodes;

//sets up allocated memory
void initialize () {
    uninitialized = 0; //active

    //allocate mem
    startLocation = malloc(sizeof(Node) * LIST_MAX_NUM_NODES);
    endLocation = startLocation + sizeof(Node) * LIST_MAX_NUM_NODES;

    //set up the emmpty nodes (a list of empty nodes)
    List

} //when program ends -> need to free mem

//frees up allocated mem
void destruct () {
    free(startLocation);
}

void printNode (Node* node) {
    //safety:
    // node
    // printf("");
}

void printBlock () {
    for (int i = 0; i < LIST_MAX_NUM_NODES; i++) {
        printNode(startLocation + i * sizeof(Node));
    }
}

//finds an unused head (list)
List* getFreeHead() {
    
}

//creating a list -> we need to make the static section before doing anything
//lists will act as "heads"
List* List_create() {
    if (uninitialized) {
        intialize();
    }

    //check if we have room -> else: NULL
    if (activeHeads >= LIST_MAX_NUM_HEADS) {
        // return NULL;
    }

    //make the list (can't use new)
    List* list = getFreeHead();


    
}


//asume user calls this on all Lists before ending
void List_free (List* pList, FREE_FN pItemFreeFn) {

}
