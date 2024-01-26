#include "list.h"
#include <stdio.h>


//static variables
static int uninitialized = 1, activeHeads = 0;

//statically allocated mem
static Node nodes[LIST_MAX_NUM_NODES];
static List heads[LIST_MAX_NUM_HEADS];

//empty nodes
static List emptyNodes;

//sets up nodes -> starts as freeNodes
void initialize () {
    uninitialized = 0; //active

    //allocate mem for nodes

    //set up the empty nodes (a list of empty nodes), last node will point to NULL
    for (int i = 0; i < LIST_MAX_NUM_NODES - 1; i++) {
        nodes[i].next = &nodes[i + 1];
        nodes[i].empty = true;
        nodes[i].val = 0; //should be NULL -> trivial since our node is empty for now
    }

    emptyNodes.current = &nodes[0];
    emptyNodes.head = &nodes[0];
    emptyNodes.tail = &nodes[LIST_MAX_NUM_NODES - 1];
    emptyNodes.n = LIST_MAX_NUM_NODES;
}

void printNode (Node* node) {
    if (node->empty) {
        printf("EMPTY");
        return;
    }

    printf("VAL"); //TO DO
}

void printAll () {
    for (int i = 0; i < LIST_MAX_NUM_NODES; i++) {
        printNode(&nodes[i]);
    }
}

//finds an unused head (list)
List* getFreeHead() {
    return NULL;
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
