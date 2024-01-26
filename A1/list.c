#include "list.h"
#include <stdio.h>


//static variables
static int uninitialized = 1, activeHeads = 0;

//statically allocated mem
static Node nodes[LIST_MAX_NUM_NODES];
static List heads[LIST_MAX_NUM_HEADS];

//empty nodes
static List emptyNodes;

//public:

void Print_all () {
    for (int i = 0; i < LIST_MAX_NUM_NODES; i++) {
        printNode(&nodes[i]);
        printf("\n");
    }
}

//finds an unused head (list) //TO DO
List* getFreeHead() {
    return &heads[0];
}

//creating a list -> we need to make the static section before doing anything
//lists will act as "heads"
List* List_create() {
    if (uninitialized) {
        initialize();
    }

    //check if we have room -> else: NULL
    if (activeHeads >= LIST_MAX_NUM_HEADS) {
        return NULL;
    }

    //make the list (can't use new)
    List* list = getFreeHead(); //should be set as empty

    return list;
}

//return NULL if no free nodes
Node* Get_free_node () {
    return NULL;
}

//0 -> success, -1 -> fail
int List_append(List* pList, void* pItem) {
    //find a free node
    Node* free = Get_free_node();

    //no nodes available
    if (free = NULL) {
        return -1;
    }
    
    //add val
    free->next = NULL; //break from emptyNodes
    free->val = pItem;
    free->empty = false; //set as occupied

    //append, condition: empty list
    if (pList->empty) {
        pList->current = pList->head = free; //tail is set later anyways
    } else {
        //last-> next = free
        pList->tail->next = free;
    }
    
    //edit List data
    pList->tail = free;
    pList->n++;
}

void List_free (List* pList, FREE_FN pItemFreeFn) {
    
}

//private:
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

    //last node
    Node *last = &nodes[LIST_MAX_NUM_NODES - 1];
    last->empty = true;
    last->next = NULL;

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
